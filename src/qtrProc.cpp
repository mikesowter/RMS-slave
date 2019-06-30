#include "extern.h"

// end of quarter processing

void qtrProc() {
  if ( minute() != oldMin ) SPISlave.setStatus(now());
  
  if ( day() != oldDay ) {
    // maintain time in SPI status register
    delay(5000);
    getTime();
    setTime(startSeconds);
    // generate new file name for day
    strcpy(todayName,"/rs");
    strcat(todayName,dateStamp());
    strcat(todayName,".csv");
    // update month and year
    oldYear = year();
    oldMonth = month();
    oldDay = day();
  }

  // storeData();
  oldHour = hour();
  oldQtr = minute()/15;
  oldMin = minute();
  // flush fault files
  fd.flush();
  fe.flush();
}
