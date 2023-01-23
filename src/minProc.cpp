
#include "extern.h"

// scheduled processing

void minProc() {
  // update master time
  SPISlave.setStatus(now());
  oldMin = minute();
  // check battery charge level at night
  t31check();
  // check for new quarter hour
  if ( oldQtr == minute()/15 ) return;
  storeData();                          // write day file every 15mins
  oldHour = hour();
  oldQtr = minute()/15;
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
    Energy[i] = 0.0;
    costEnergy[i] = 0.0;
  }
  T11_kWh = 0.0;
  batt_tohouse = 0.0;
  batt_togrid = 0.0;
  batt_savings = 0.0;
  batt_costs = 0.0;
  batt_tohouse75 = 0.0;
  batt_togrid75 = 0.0;
  batt_savings75 = 0.0;
  batt_costs = 0.0;
  return;
}  
