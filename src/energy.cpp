#include "extern.h"
#include "defines.h"



float NOISE[] = {5,5,5,5,5,5,50,15,5};  // updated 20220725 to handle oven(6) noise

// Energy sums are reset at midnight in minProc

void dailyEnergy() {

#ifdef RMS1
  float tier1loads, tier2loads, split, rate, noise = 1.5E-6; // (5W for 1s)
  float tier1solar, tier2solar, spareSolar, factor = 1.0F;
#endif
  t_scan = millis() - t_lastData;
  t_lastData = millis();
  tier1loads = 0.0F;
  for ( int i = 1;i<NUM_CCTS+1;i++ ) {
    if ( Wrms[i] < NOISE[i] ) Wrms[i] = 0.0;          // eliminate noise
    incEnergy[i] = Wrms[i]*(float)t_scan/3.6e9;       // kWh units
    Energy[i] += incEnergy[i];
#ifdef RMS1
    if ( i!=1 && i!=5 && i!=7 ) tier1loads += incEnergy[i]; // loads 2,3,4,6,8
#endif
  }
#ifdef RMS1
  loads = incEnergy[1];     // total kWh (solar+T11) on dist panel
  // calculate the impact of 3 panel sizes
  for ( uint8_t ps=0;ps<3;ps++) {                      
    solar = factor*incEnergy[7];                       // simulating panels of 5,7.5 and 10kW
    tier2loads = loads - tier1loads;                   // non-essential (e.g. big heat pumps)
    if ( solar > loads ) {
      tier1solar = tier1loads;
      tier2solar = tier2loads;
      spareSolar = solar - loads;
    }
    else if ( solar > tier1loads ) {
      tier1solar = tier1loads;
      tier2solar = solar - tier2loads;
      spareSolar = 0.0F;
    }
    else {
      tier1solar = solar - tier1loads;
      tier2solar = 0.0F;
      spareSolar = 0.0F;
    }
    T11_inc[ps] = 0.0F;
 
    for ( int i = 2;i<NUM_CCTS+1;i++ ) {
      if ( i == 5 && waterOn ) {
        costEnergy[ps][i] += T31 * incEnergy[5];      // hotwater tariff
      }
      else if ( i == 7 ) {
        costEnergy[ps][7] += FIT * spareSolar;        // export unuseable solar
      }
      else if ( solar > tier1loads ) {                // all provided by solar
        costEnergy[ps][i] += FIT * incEnergy[i];
      }
      else if ( solar < noise ) {
        costEnergy[ps][i] += T11 * incEnergy[i];      // none provided by solar
        T11_inc[ps] += incEnergy[i];
      }
      else if ( tier1loads > noise ) {
        split = min(1.0F,tier1solar/tier1loads);      // loads metered separately
        rate = FIT * split + T11 * (1.0F - split);    // (2,3,4,6,8) are essential
        costEnergy[ps][i] += rate * incEnergy[i];     // use first portion of solar
        T11_inc[ps] += incEnergy[i] * (1.0F - split); 
      }
      if ( tier2loads > noise ) {                     // remove noise & zero from calc
        split = min(1.0F,tier2solar/tier2loads);      // use second portion of solar
        rate = FIT * split + T11 * (1.0 - split);
        costEnergy[ps][1] += rate * tier2loads;
        T11_inc[ps] += tier2loads * (1.0F - split); 
      }
    }
    T11_kWh[ps] += T11_inc[ps];
    T11_kWh[ps] = max(0.0F,loads - solar);
    FIT_kWh[ps] = max(0.0F,Energy[7]-Energy[1]);
    factor += 0.5F;
  }
  //T11_kW = (T11_inc[0]/float(t_scan))*3600000.0F;
  T11_kW = max(0.0F,Wrms_min[1] - Wrms_min[7])/1000.0F;
#endif
}

