#include <extern.h>

float NOISE[] = {5,5,5,5,5,5,50,15,5};  // updated 20220725 to handle oven(6) noise

// Energy sums are reset at midnight in minProc

void dailyEnergy() {

  float goodLoads, badLoads, split, rate;
  t_scan = millis() - t_lastData;
  t_lastData = millis();
  goodLoads = 0.0;
  for ( int i = 1;i<NUM_CIRCUITS+1;i++ ) {
    if ( Wrms[i] < NOISE[i] ) Wrms[i] = 0.0;  // eliminate noise
    incEnergy[i] = Wrms[i]*(float)t_scan/3.6e9;       // kWh units
    Energy[i] += incEnergy[i];
    if ( i!=1 && i!=5 && i!=7 ) goodLoads += incEnergy[i]; // water&solar
  }
  loads = incEnergy[1];     // T11 incoming to dist panel
  solar = incEnergy[7];     // inverter incoming to dist panel
  float spareSolar = solar - loads; 

  for ( int i = 2;i<NUM_CIRCUITS+1;i++ ) {
    if ( i == 5 && waterOn ) {
      costEnergy[i] += T31 * incEnergy[i];        // hotwater tariff
    }
    else if ( i == 7 ) {
      costEnergy[i] += FIT * max(0.0F,solar-loads); // exported solar
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
         
  if ( exporting ) T11_inc = 0.0;   // just for clarity
  else {
    T11_inc = max(0.0F,loads-solar);
    T11_kWh += T11_inc;
  }

  if ( exporting75 ) T11_inc75 = 0.0;   // imaginery panels
  else {
    T11_inc75 = max(0.0F,loads-solar*1.5F);
    T11_kWh75 += T11_inc75;
  }
}
