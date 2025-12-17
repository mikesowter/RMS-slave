#include "extern.h"
#include "defines.h"

#ifdef RMS1
float noise[] = {0,5,5,5,5,5,50,5,5};  // updated 20220725 to handle oven(6) noise
float tier1loads = 0.0F, tier2loads, split, rate, T11_rate, FIT_rate; 
float tier1solar, tier2solar, spareSolar, factor;
float usSum;
#else
float noise[] = {0,5,5,5,5,5,5,5}; 
#endif

// Energy sums are reset at midnight in minProc
// all energy is expressed in kWh, so 1s increments are very small (>4E-6)

void dailyEnergy() {

  t_scan = micros()-t_lastData;         // typically 900ms for RMS1, 400ms for RMS2
  t_lastData = micros();
 
  if ( t_scan > t_scan_max ) t_scan_max = t_scan; 
  if ( t_scan < t_scan_min ) t_scan_min = t_scan;     //             us-s   W-kW   s-hr
  float Wus2kWh = (float)t_scan/3.6E12;                // Wus to kWh (E-6)*(E-3)*(1/3600)
  

  for ( int i = FIRST_CCT;i<NUM_CCTS;i++ ) {               
    if ( abs(Wrms[i]) < noise[i] ) Wrms[i] = 0.0;     // eliminate noise
    incEnergy[i] = Wrms[i] * Wus2kWh;   
    if ( abs(incEnergy[i]) < 0.003F ) {               // check for excessive energy calcs
      Energy[i] += incEnergy[i];  
#ifdef RMS1
      if ( i!=1 && i!=5 && i!=7 ) tier1loads += incEnergy[i]; // loads 2,3,4,6,8 (CCTS, oven & lights)
#endif
    }
  }
#ifdef RMS2
  Energy[0] += Wimp * Wus2kWh;                    // daily sum energy 
  Energy[7] += Wexp * Wus2kWh;                    // export defined as neg power flow
      
  Imp_meter += Wimp * Wus2kWh;                    // energex meter
  Exp_meter += Wexp * Wus2kWh;
#endif
#ifdef RMS1
  loads = incEnergy[1];                           // total inc kWh (solar+Tariff11) load on dist panel
  // calculate the impact of 3 panel sizes
  factor = 1.0F;                                  // simulating panels of 5,7.5 and 10kW
  for ( uint8_t ps=0;ps<3;ps++) {                      
    solar = factor*incEnergy[7];                  // solar inc kWh (over the last scan period t_scan ) 
    tier2loads = loads - tier1loads;              // non-essential (e.g. big heat pumps)
    if ( solar > loads ) {
      tier1solar = tier1loads;
      tier2solar = tier2loads;
    }
    else if ( solar > tier1loads ) {
      tier1solar = tier1loads;
      tier2solar = solar - tier1loads;
    }
    else {
      tier1solar = solar;
      tier2solar = 0.0F;
    }
    spareSolar = solar - loads;                           // inc kWh, +ive if excess solar after loads 
    // calculate tier1 costs
    split = tier1solar/tier1loads;  
    bool weekend = ( weekday() == 1 || weekday() == 7 );
    if ( !weekend && hour() >= 16 && hour() < 20 ) {      // peak is 4pm to 8pm weekdays
      T11_rate = T11_high;
      FIT_rate = FIT_peak;                                // FIT_rate is only used in battery simulation of wholesale market
    } 
    else if ( hour() >= 7 && hour() < 22 ) {              // day is 7am to 10pm
      T11_rate = T11_med;     
      FIT_rate = FIT_day;   
    } 
    else  {                                               // night is 10pm to 7am
      T11_rate = T11_low;
      FIT_rate = FIT_night;
    }

    rate = FIT_rate * split + T11_rate * (1.0F - split);       

    for ( int i = 2;i<NUM_CCTS+1;i++ ) {
      if ( i == 5 && waterOn ) {
        costEnergy[ps][i] += T31 * incEnergy[5];      // hotwater tariff
      }
      else if ( i == 7 ) {
        costEnergy[ps][7] += FIT_rate * max(0.0F,spareSolar);   // export unuseable solar (if any)
      }
      else {
        costEnergy[ps][i] += rate * incEnergy[i];     // use first portion of solar
      }
    }
    T11_inc[ps] = tier1loads - tier1solar; 
    // calculate tier2 costs
    if ( tier2loads > 4E-6 ) {                        // loads lumped together >5W
      split = tier2solar/tier2loads;                  // use second portion of solar
      rate = FIT_rate * split + T11_rate * (1.0F - split);   
      costEnergy[ps][1] += rate * tier2loads;         // cost of total load
      T11_inc[ps] += (tier2loads - tier2solar);
    }
    if ( ps > 0 ) T11_kWh[ps] += T11_inc[ps];         // see below for ps=0
    FIT_kWh[ps] +=  max(0.0F,spareSolar);
    
    factor += 0.5F;                                   // next panel size emulation
  }
  // this is a new simple calc for T11 energy with existing solar
  T11_W = max(0.0F,Wrms_min[1] - Wrms_min[7]);
  float T11_kWh_inc = T11_W * Wus2kWh;
  T11_kWh[0] += T11_kWh_inc;
#endif
}

