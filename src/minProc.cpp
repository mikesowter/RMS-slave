
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
  // update time
  if ( minute() == 0 ) return;    // waits till 00:01
  setTime( getTime() );
  // generate new file name for day
  strcpy(todayName,"/rm");
  strcat(todayName,dateStamp());
  strcat(todayName,".csv");
  // update month and year
  oldYear = year();
  oldMonth = month();
  oldDay = day();
  return;
}  
