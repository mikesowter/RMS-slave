#include "extern.h"
#include "defines.h"



float NOISE[] = {5,5,5,5,5,5,50,15,5};  // updated 20220725 to handle oven(6) noise

// Energy sums are reset at midnight in minProc

void dailyEnergy() {

#ifdef RMS1
    float goodLoads=0.0F, badLoads, split, rate;
#endif
  t_scan = millis() - t_lastData;
  t_lastData = millis();
  for ( int i = 1;i<NUM_CCTS+1;i++ ) {
    if ( Wrms[i] < NOISE[i] ) Wrms[i] = 0.0;          // eliminate noise
    incEnergy[i] = Wrms[i]*(float)t_scan/3.6e9;       // kWh units
    Energy[i] += incEnergy[i];
#ifdef RMS1
    if ( i!=1 && i!=5 && i!=7 ) goodLoads += incEnergy[i]; // loads 2,3,4,6,8
#endif
  }
#ifdef RMS1
  loads = incEnergy[1];     // total load (solar+T11) on dist panel
  solar = incEnergy[7];     // inverter incoming to dist panel
  float goodSolar, badSolar, spareSolar, factor = 1.0F;
  for ( uint8_t ps=0;ps<3;ps++) {                   // calculate the impact of 3 panel sizes
    solar = factor*incEnergy[7];                    // simulating panels of 5,7.5 and 10kW
    badLoads = loads - goodLoads;                   // non-essential (big heat pumps)
    goodSolar = max(0.0F,(solar-goodLoads)); 
    badSolar = max(0.0F,(solar-badLoads));
    spareSolar = max(0.0F,(solar-loads));
    T11_inc[ps] = 0.0F;
 
    for ( int i = 2;i<NUM_CCTS+1;i++ ) {
      if ( i == 5 && waterOn ) {
        costEnergy[ps][i] += T31 * incEnergy[i];      // hotwater tariff
      }
      else if ( i == 7 ) {
        costEnergy[ps][i] += FIT * spareSolar;        // exported solar
      }
      else if ( solar > goodLoads ) {              // all provided by solar
        costEnergy[ps][i] += FIT * incEnergy[i];
      }
      else if ( solar == 0.0F ) {
        costEnergy[ps][i] += T11 * incEnergy[i];      // none provided by solar
        T11_inc[ps] += incEnergy[i];
      }
      else if ( goodLoads > 0.0F ) {
        split = min(1.0F,goodSolar/goodLoads);     // loads metered separately
        rate = FIT * split + T11 * (1.0F - split);    // (2,3,4,6,8) are essential
        costEnergy[ps][i] += rate * incEnergy[i];     // use first portion of solar
        T11_inc[ps] += incEnergy[i] * (1.0F - split); 
      }
      if (badLoads > 2.5E-4) {                        // remove noise & zero from calc
        split = min(1.0F,badSolar/badLoads);        // use last portion of solar
        rate = FIT * split + T11 * (1.0 - split);
        costEnergy[ps][1] += rate * badLoads;
        T11_inc[ps] += badLoads * (1.0F - split); 
      }
    }
    T11_kWh[ps] += T11_inc[ps];
    T11_kW = (T11_inc[0]/float(t_scan))*3600000.0F;
    factor += 0.5F;
    FIT_kWh[ps] = max(0.0F,Energy[7]-Energy[1]);
  }
#endif
}

