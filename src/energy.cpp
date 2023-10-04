#include <extern.h>

#define T31 0.2138    // updated 20230701
#define T11 0.3267    // updated 20230701
#define FIT 0.08      // updated 20211128

float T11_kWh[3];           // daily sum from grid with each panel size
float T11_inc[3];           // increment from grid

float NOISE[] = {5,5,5,5,5,5,50,15,5};  // updated 20220725 to handle oven(6) noise

// Energy sums are reset at midnight in minProc

void dailyEnergy() {

  float goodLoads, badLoads, split, rate;
  t_scan = millis() - t_lastData;
  t_lastData = millis();
  goodLoads = 0.0;
  for ( int i = 1;i<NUM_CCTS+1;i++ ) {
    if ( Wrms[i] < NOISE[i] ) Wrms[i] = 0.0;          // eliminate noise
    incEnergy[i] = Wrms[i]*(float)t_scan/3.6e9;       // kWh units
    Energy[i] += incEnergy[i];
    if ( i!=1 && i!=5 && i!=7 ) goodLoads += incEnergy[i]; // water&solar
  }
  #ifndef RMS2
  loads = incEnergy[1];     // T11 incoming to dist panel
  solar = incEnergy[7];     // inverter incoming to dist panel
  float spareSolar = solar - loads; 

  for ( int i = 2;i<NUM_CCTS+1;i++ ) {
    if ( i == 5 && waterOn ) {
      costEnergy[i] += T31 * incEnergy[i];        // hotwater tariff
    }
    else if ( i == 7 ) {
      costEnergy[i] += FIT * max(0.0F,spareSolar); // exported solar
    }
    else if ( solar > loads ) {                   // all provided by solar
      costEnergy[i] += FIT * incEnergy[i];
    }
    else if ( solar == 0.0 ) {
      costEnergy[i] += T11 * incEnergy[i];        // none provided by solar
    }
    else if ( goodLoads > 0.0F ) {
      split = min(1.0F,solar/goodLoads);          // loads metered separately
      rate = FIT * split + T11 * (1.0F - split);  // (2,3,4,6,8) are essential
      costEnergy[i] += rate * incEnergy[i];       // use first portion of solar
    }
  }
  badLoads = loads - goodLoads;                   // non-essential
  if (badLoads > 2.5E-4) {                        // remove noise from subtraction
    spareSolar = max(0.0F,(solar-goodLoads));      
    split = min(1.0F,spareSolar/badLoads);        // use next portion of solar
    rate = FIT * split + T11 * (1.0 - split);
    costEnergy[1] += rate * badLoads;
  }

// all above is calc'ed on real values, 5kW and no battery. The next calc's the impact of simulated extra panels on T11

  float fact = 1.0F;
  for (uint8_t ps=0;ps<3;ps++) {      
    T11_inc[ps] = max(0.0F,loads-(solar*fact));
    T11_kWh[ps] += T11_inc[ps];
    fact += 0.5F;
  }
  #endif
}
