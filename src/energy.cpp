#include <extern.h>

#define T31 0.18172   // updated 20240701
#define T11 0.2717    // updated 20240701
#define FIT 0.08      // updated 20211128
#define Dem 0.18546   // updated 20240701

float T11_kWh[3];           // daily sum from grid with each panel size
float T11_inc[3];           // increment from grid
float FIT_kWh[3];
float FIT_inc[3];

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
  float spareSolar, factor = 1.0F;
  for ( uint8_t ps=0;ps<3;ps++) {                   // calculate the impact of 3 panel sizes
    spareSolar = max(0.0F,(solar*factor-goodLoads)); 
 
    for ( int i = 2;i<NUM_CCTS+1;i++ ) {
      if ( i == 5 && waterOn ) {
        costEnergy[ps][i] += T31 * incEnergy[i];      // hotwater tariff
      }
      else if ( i == 7 ) {
        costEnergy[ps][i] += FIT * spareSolar;        // exported solar
      }
      else if ( solar > loads ) {                     // all provided by solar
        costEnergy[ps][i] += FIT * incEnergy[i];
      }
      else if ( solar == 0.0 ) {
        costEnergy[ps][i] += T11 * incEnergy[i];      // none provided by solar
      }
      else if ( goodLoads > 0.0F ) {
        split = min(1.0F,solar/goodLoads);            // loads metered separately
        rate = FIT * split + T11 * (1.0F - split);    // (2,3,4,6,8) are essential
        costEnergy[ps][i] += rate * incEnergy[i];     // use first portion of solar
      }
    
      badLoads = loads - goodLoads;                   // non-essential
      if (badLoads > 2.5E-4) {                        // remove noise from subtraction
        split = min(1.0F,spareSolar/badLoads);        // use next portion of solar
        rate = FIT * split + T11 * (1.0 - split);
        costEnergy[ps][1] += rate * badLoads;
      }
    }
    factor += 0.5F;
  }

/* all above is calc'ed on real values, 5kW panels and no battery. 
// The next calc's the impact on T11 & FIT of extra panels and no battery

  float solarToGrid, fact = 1.0F;
  loads = incEnergy[1];     // total load (some solar+T11) on dist panel
  solar = incEnergy[7];     // solar incoming to house and grid
  for (uint8_t ps=0;ps<3;ps++) {      
    solarToGrid = solar*fact - loads;
    T11_inc[ps] = max(0.0F,-solarToGrid);
    T11_kWh[ps] += T11_inc[ps];
    FIT_inc[ps] = max(0.0F,solarToGrid);
    FIT_kWh[ps] += FIT_inc[ps];
    fact += 0.5F; */

#endif
}

