#include "extern.h"
#include "defines.h"


#ifdef RMS1
float noise[] = {0,5,5,5,5,5,50,5,5};  // updated 20220725 to handle oven(6) noise
float tier1loads = 0.0F, tier2loads, split, rate; 
float tier1solar, tier2solar, spareSolar, factor;
#else
float noise[] = {0,5,5,5,5,5,5,5}; 
#endif

// Energy sums are reset at midnight in minProc
// all energy is expressed in kWh, so 1s increments are very small (>4E-6)

void dailyEnergy() {

  t_scan = max( 300UL, millis()-t_lastData );         // typically 900ms for RMS1, 400ms for RMS2
  // t_scan = min( 1000UL, t_scan );
  if ( t_scan > t_scan_max ) t_scan_max = t_scan; 
  if ( t_scan < t_scan_min ) t_scan_min = t_scan;     //              ms-s     W-kW     s-hr
  float Wms2kWh = (float)t_scan/3.6E9;                // Wms to kWh (1/1000)*(1/1000)*(1/3600)
  
  t_lastData = millis();
  for ( int i = FIRST_CCT;i<NUM_CCTS;i++ ) {                  // power (W) to energy (kWh)
    if ( abs(Wrms[i]) < noise[i] ) Wrms[i] = 0.0;     // eliminate noise
    incEnergy[i] = Wrms[i] * Wms2kWh;   
    if ( abs(incEnergy[i]) < 0.003F ) Energy[i] += incEnergy[i];  // 10000W*1000ms/3.6E9
    else {
      sprintf(charBuf,"excess incEnergy[%d] of %.3f",i,incEnergy[i]);
      diagMess(charBuf);
    }
#ifdef RMS1
    if ( i!=1 && i!=5 && i!=7 ) tier1loads += incEnergy[i]; // loads 2,3,4,6,8
#endif
  }
#ifdef RMS2
  Energy[0] += Wimp * Wms2kWh;                    // daily sum energy 
  Energy[7] += Wexp * Wms2kWh;                    // export defined as neg power flow
      
  Imp_meter += Wimp * Wms2kWh;                    // energex meter
  Exp_meter += Wexp * Wms2kWh;
#endif
#ifdef RMS1
  loads = incEnergy[1];     // total kWh (solar+Tariff11) on dist panel
  // calculate the impact of 3 panel sizes
  factor = 1.0F;
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
      tier2solar = solar - tier1loads;
      spareSolar = 0.0F;
    }
    else {
      tier1solar = solar;
      tier2solar = 0.0F;
      spareSolar = 0.0F;
    }
    // calculate tier1 costs
    split = tier1solar/tier1loads;  
    rate = FeedInTariff * split + Tariff11 * (1.0F - split);        // (2,3,4,6,8) are essential
    for ( int i = 2;i<NUM_CCTS+1;i++ ) {
      if ( i == 5 && waterOn ) {
        costEnergy[ps][i] += T31 * incEnergy[5];      // hotwater tariff
      }
      else if ( i == 7 ) {
        costEnergy[ps][7] += FeedInTariff * spareSolar;        // export unuseable solar
      }
      else {
        costEnergy[ps][i] += rate * incEnergy[i];     // use first portion of solar
      }
    }
    T11_inc[ps] = tier1loads - tier1solar; 
    // calculate tier2 costs
    if ( tier2loads > 4E-6 ) {                        // loads lumped together >5W
      split = tier2solar/tier2loads;                  // use second portion of solar
      rate = FeedInTariff * split + Tariff11 * (1.0F - split);
      costEnergy[ps][1] += rate * tier2loads;         // cost of total load
      T11_inc[ps] += (tier2loads - tier2solar);
    }
  //  T11_kWh[ps] += T11_inc[ps];
    FIT_kWh[ps] += spareSolar;
    factor += 0.5F;                                   // next panel size emulation
  }
  // this is a power calc, not energy, for debugging purposes only
  T11_W = max(0.0F,Wrms_min[1] - Wrms_min[7]);
  float T11_kWh_inc = T11_W * Wms2kWh;
  T11_kWh[0] += T11_kWh_inc;
#endif
}

