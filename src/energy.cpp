#include <extern.h>

const float T31 = 0.166;   // updated 20220901
const float T11 = 0.241;   // updated 20220901
const float FIT = 0.08;    // updated 20211128
float NOISE[] = {5,5,5,5,5,5,50,10,5};  // updated 20220725 to handle oven(6) noise

// Energy sums are reset at midnight in minProc

void dailyEnergy() {

  float goodLoads, badLoads, loads, solar, spareSolar, split, rate;
  extern float T11_kWh,T11_inc;

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
  
  if ( solar < 0.00001 ) solar = 0.0;

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
    else {
      split = min(1.0F,solar/goodLoads);          // loads metered separately
      rate = FIT * split + T11 * (1.0 - split);   // (2,3,4,6,8) are essential
      costEnergy[i] += rate * incEnergy[i];       // use first portion of solar
    }
  }
  badLoads = loads - goodLoads;                   // non-essential
  if (badLoads < 2.5E-4) badLoads = 0.0;          // remove noise from subtraction
  spareSolar = max(0.0F,(solar-goodLoads));      
  split = min(1.0F,spareSolar/badLoads);          // use next portion of solar
  rate = FIT * split + T11 * (1.0 - split);
  costEnergy[1] += rate * badLoads;
         
  if ( exporting ) T11_inc = 0.0;   // just for clarity
  else {
    T11_inc = max(0.0F,loads-solar);
    T11_kWh += T11_inc;
  }
  
  //  sprintf(longStr,"loads: %f,solar: %f,Feed In: %f,T11_inc: %f,T11_kWh: %f",loads,solar,spareSolar,T11_inc,T11_kWh);
  //  diagMess(longStr);
}
