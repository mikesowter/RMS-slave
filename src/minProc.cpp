
#include "extern.h"
extern float T11_kWh[3];

// scheduled processing

void minProc() {
  // update master time
  SPISlave.setStatus(now());
  oldMin = minute();
  // check battery charge level at night
  // t31check();
  // check for new quarter hour
  if ( oldQtr == minute()/15 ) return;
  storeData();                          // write day file every 15mins
  oldHour = hour();
  oldQtr = minute()/15;
  badSumCount = 0;
  // check for end of day
  if ( day() == oldDay ) return;
  // update time at 00:00:02
  delay(2000);
  setupTime();
  // write days energy totals
  updateEnergyFile();
  // battery simulation totals
  updateBatteryFile();
  // reset daily energy sums at midnight
  for ( int i = 1; i<NUM_CIRCUITS+1; i++ ) {
    Energy[i] = 0.0F;
    costEnergy[i] = 0.0F;
  }

  for (uint8_t ps = 0;ps<3;ps++) {
  // then through battery size (bs)
    for (uint8_t bs = 0;bs<3;bs++) {
      batt_tohouse[ps][bs] = 0.0F;
      dump_togrid[ps][bs] = 0.0F;
      batt_savings[ps][bs] = 0.0F;
    }
    T11_kWh[ps] = 0.0F;
  }
  return;
}  
