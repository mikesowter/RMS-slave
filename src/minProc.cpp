
#include "extern.h"

// end of quarter processing

void minProc() {
  // update master time
  SPISlave.setStatus(now());
  oldMin = minute();
  // flush fault files
  fd.flush();
  fe.flush();
  // check for new quarter hour
  if ( oldQtr == minute()/15 ) return;
  storeData();                    // write day file every 15mins
  oldHour = hour();
  oldQtr = minute()/15;
  // check for end of day
  if ( day() == oldDay ) return;
  // update time at 00:00:02
  delay(2000);
  setupTime();
  // write days energy totals
  updateEnergyFile();
  // reset daily energy sums at midnight
  if ( hour() == 0) {
    for ( int i = 1;i<NUM_CHANNELS+1;i++ ) Energy[i] = 0.0;
  }
  return;
}  
