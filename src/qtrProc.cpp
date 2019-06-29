#include "C:\Users\Mikes 6700K\Dropbox\sketchbook\RMS slave\src\extern.h"

// end of quarter processing

void qtrProc() {
  diagMess((char*)"qtr processing");
  // maintain time in SPI status register
  SPISlave.setStatus(now());

  watchDog = 0;
  delay(10);

  if ( day() != oldDay ) {
    delay(5000);
    getTime();
    setTime(startSeconds);

    // generate new file name for day
    strcpy(todayName,"/rs");
    strcat(todayName,i2sd(year()%100));
    strcat(todayName,i2sd(month()));
    strcat(todayName,i2sd(day()));
    strcat(todayName,".csv");
//    storeData();
    // update month and year
    oldYear = year();
    oldMonth = month();
    oldDay = day();
  }
  oldHour = hour();
  oldQtr = minute()/15;
  Vmin=0;
  Vmax=0;
  // flush fault files
  fd.flush();
  fe.flush();
}
