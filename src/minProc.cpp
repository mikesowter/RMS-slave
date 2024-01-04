
#include "extern.h"

// scheduled processing

void minProc() {
  // update master time
  SPISlave.setStatus(now());
  if ( minute() == oldMin ) return;
  oldMin = minute();
  // check for new quarter hour
  if ( oldQtr == minute()/15 ) return;
  storeData();                          // write day file every 15mins
  oldHour = hour();
  oldQtr = minute()/15;
  badSumCount = 0;
  // check for 5am for battery sim reset
#ifdef RMS1
  if ( hour() == 5 && minute() == 0 ) {
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
    // reset daily energy costs
    for ( int i = 1; i<NUM_CCTS+1; i++ ) {
      costEnergy[i] = 0.0F;
    }
  }
#endif
  // check for end of day
  if ( day() == oldDay ) return;
  // update time at 00:00:05
  delay(5000);
  setupTime();
  // write days energy totals
  updateEnergyFile();
#ifdef RMS1
  // battery simulation totals
  updateBatteryFile();
#endif
  // reset daily energy sums at midnight
  for ( int i = 1; i<NUM_CCTS+1; i++ ) {
    Energy[i] = 0.0F;
  //  costEnergy[i] = 0.0F;
  }

  return;
}  
