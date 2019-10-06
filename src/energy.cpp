#include <extern.h>

const float T31 = 0.1327;
const float T11 = 0.2018;
const float FIT = 0.11;
const float NOISE = 8.0;

// Energy sums are reset at midnight in minProc

void dailyEnergy() {
  float incEnergy[NUM_CHANNELS+1];
  float goodLoads, badLoads, loads, solar, spareSolar, split, rate;

  t_scan = millis() - t_lastData;
  t_lastData = millis();
  goodLoads = 0.0;
  for ( int i = 1;i<NUM_CHANNELS+1;i++ ) {
    if ( Wrms[i] > NOISE ) {                          // eliminate noise
      incEnergy[i] = (float)t_scan/3.6e9;             // kWh units
      Energy[i] += incEnergy[i];
      if ( i!=4 && i!=7 ) goodLoads += incEnergy[i];    // 4=water 7=solar
    }
  }
  loads = incEnergy[1];     // define for readability
  solar = incEnergy[7];

  for ( int i = 1;i<NUM_CHANNELS+1;i++ ) {
    if ( i == 4 ) {
      costEnergy[i] += T31 * incEnergy[i];          // hotwater tariff
    }
    else if ( i == 7 ) {
      costEnergy[i] += FIT * max((float)0,solar-loads);    // exported solar
    }
    else if ( solar > loads ) {                     // if all provided by solar
      costEnergy[i] += FIT * incEnergy[i];
    }
    else if ( solar < NOISE ) {
      costEnergy[i] += T11 * incEnergy[i];          // none provided by solar
    }
    else {
      if ( i != 1 ) {
        split = min((float)1,solar/goodLoads);      // loads metered separately
        rate = FIT * split + T11 * (1.0 - split);   // are essential
        costEnergy[i] += rate * incEnergy[i];       // get first portion of solar
      }
      else {
        badLoads = loads - goodLoads;                 // loads not metered separately
        spareSolar = max((float)0,(solar-goodLoads)); // are non-essential
        split = min((float)1,spareSolar/badLoads);    // get next portion of solar
        rate = FIT * split + T11 * (1.0 - split);
        costEnergy[i] += rate * badLoads;          
      }
    }
  }
}