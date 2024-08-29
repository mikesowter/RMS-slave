
#include "extern.h"

uint8_t en5index, en15index;
void writePeak();

// scheduled processing

void minProc() {
  // update master time
  SPISlave.setStatus(now());
  if ( minute() == oldMin ) return;
  oldMin = minute();
  if ( old5Min == minute()/5 ) return;
  old5Min = minute()/5;
  #ifdef RMS1
  // demand calcs
  if ( hour() == 16 && minute() == 30 ) peakPeriod = true;
  en5index = (minute()/5)%6;    // index into barrel of 6 x 5 min T11 readings
  en15index = (en5index+3)%6;   // index 15 min back
  rms15Demand = max(0.0F, Energy[1] - en5min[en15index]);
  rms15Demand -= max(0.0F,Energy[7] - so5min[en15index]);
  rms15Demand = max(0.0F,rms15Demand)*4.0F;
  if ( peakPeriod && rms15Demand > rms15Peak ) rms15Peak = rms15Demand;
  rms30Demand = (Energy[1] - en5min[en5index])*2.0F;  
  rms30Demand -= (Energy[7] - so5min[en5index])*2.0F;
  rms30Demand = max(0.0F,rms30Demand);
  if ( peakPeriod && rms30Demand > rms30Peak ) rms30Peak = rms30Demand;
  en5min[en5index] = Energy[1]; // overwrite values from 30m ago
  so5min[en5index] = Energy[7];
  #endif
  // check for new quarter hour
  if ( oldQtr == minute()/15 ) return;
  oldQtr = minute()/15;
  storeData();                   // write day file every 15mins
  if ( oldHour == hour() ) return;
  oldHour = hour();
  if ( hour() == 21 ) {
    writePeak();
    peakPeriod = false;
  }
  
  // check for end of day
  if ( day() == oldDay ) return;
  // update time at 00:00:00
  setupTime();
  // write days energy totals
  updateEnergyFile();
#ifdef RMS1
  // battery simulation totals
  updateBatteryFile();
  // iterate through panel size (ps)
  for (uint8_t ps = 0;ps<3;ps++) {
    // then through battery size (bs)
    for (uint8_t bs = 0;bs<3;bs++) {
      batt_tohouse[ps][bs] = 0.0F;
      dump_togrid[ps][bs] = 0.0F;   
      batt_savings[ps][bs] = 0.0F;
    }
    T11_kWh[ps] = 0.0F;
  }
  for ( uint8_t en5index=0;en5index<6;en5index++ ) {
    en5min[en5index] = 0.0F;
    so5min[en5index] = 0.0F;
  }
  rms15Peak = 0.0F;
  rms30Peak = 0.0F;
#endif
  // reset daily energy sums at midnight
  for ( int i = 1; i<NUM_CCTS+1; i++ ) {
    Energy[i] = 0.0F;
    costEnergy[0][i] = 0.0F;
    costEnergy[1][i] = 0.0F;
    costEnergy[2][i] = 0.0F;
  }

return;
}  
