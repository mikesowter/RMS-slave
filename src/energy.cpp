#include <extern.h>

const float T31 = 0.1327;
const float T11 = 0.2018;
const float FIT = 0.11;
const float NOISE = 5.0;  // updated 20200525

// Energy sums are reset at midnight in minProc

void dailyEnergy() {

  float goodLoads, badLoads, loads, solar, spareSolar, split, rate;
  extern float T11_kWh,T11_inc;

  t_scan = millis() - t_lastData;
  t_lastData = millis();
  goodLoads = 0.0;
  for ( int i = 1;i<NUM_CIRCUITS+1;i++ ) {
    if ( Wrms[i] > NOISE ) {                            // eliminate noise
      incEnergy[i] = Wrms[i]*(float)t_scan/3.6e9;       // kWh units
      Energy[i] += incEnergy[i];
      if ( i!=1 && i!=5 && i!=7 ) goodLoads += incEnergy[i];    // 5=water 7=solar
    }
  }
  loads = incEnergy[1];     // define for readability
  solar = incEnergy[7];
  
  if ( solar < 0.00001 ) solar = 0.0;

  for ( int i = 2;i<NUM_CIRCUITS+1;i++ ) {
    if ( i == 5 && waterOn ) {
      costEnergy[i] += T31 * incEnergy[i];          // hotwater tariff
    }
    else if ( i == 7 ) {
      costEnergy[i] += FIT * max(0.0F,solar-loads);    // exported solar
    }
    else if ( solar > loads ) {                     // if all provided by solar
      costEnergy[i] += FIT * incEnergy[i];
    }
    else if ( solar == 0.0 ) {
      costEnergy[i] += T11 * incEnergy[i];          // none provided by solar
    }
    else {
      split = min(1.0F,solar/goodLoads);          // loads metered separately
      rate = FIT * split + T11 * (1.0 - split);   // are essential
      costEnergy[i] += rate * incEnergy[i];       // use first portion of solar
    }
  }
  badLoads = loads - goodLoads;                   // bad loads are non-essential
  if (badLoads < 2.5E-4) badLoads = 0.0;          // remove noise from subtraction
  spareSolar = max(0.0F,(solar-goodLoads));      
  split = min(1.0F,spareSolar/badLoads);          // use next portion of solar
  rate = FIT * split + T11 * (1.0 - split);
  costEnergy[1] += rate * badLoads;
         
  if ( exporting ) T11_inc = 0.0;
  else T11_inc = max(0.0F,loads-solar);
  
  if ( !exporting ) {   
    T11_kWh += T11_inc;
  //  sprintf(longStr,"loads: %f,solar: %f,Feed In: %f,T11_inc: %f,T11_kWh: %f",loads,solar,spareSolar,T11_inc,T11_kWh);
  //  diagMess(longStr);
  }
}
