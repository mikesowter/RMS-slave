/*
#include "extern.h"

// end of quarter processing

void minProc() {
  // update master time
  SPISlave.setStatus(now());
  oldMin = minute();
  // write day file every 15mins
  if ( oldQtr == minute()/15 ) return;
  storeData();
  oldHour = hour();
  oldQtr = minute()/15;
  // check for end of day
  if ( day() == oldDay ) return;
  // update time
  delay(5000);
  setTime(getTime());
  // generate new file name for day
  strcpy(todayName,"/rs");
  strcat(todayName,dateStamp());
  strcat(todayName,".csv");
  // update month and year
  oldYear = year();
  oldMonth = month();
  oldDay = day();
  
  // flush fault files
  fd.flush();
  fe.flush();
}  */
