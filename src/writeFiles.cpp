#include "C:\Users\Mikes 6700K\Dropbox\sketchbook\RMS slave\src\extern.h"

byte openFile(const char* s);
void WriteQtr();

byte storeData() {
  strcpy(fileName,todayName);
  if ( !openFile("a") ) return 0;
  WriteQtr();
  fh.close();
  diagMess(fileName);
  return 1;
}

//----------------- open file for reading or appending

byte openFile(const char* s) {
  fh = SPIFFS.open(fileName, s);
  if (!fh) {
    strcpy(charBuf,fileName);
    strcat(charBuf," failed to open");
    diagMess(charBuf);
    return 0;
  }
  return 1;
}

void WriteQtr() {
  fh.print(i2sd(hour()));
  fh.print(":");
  fh.print(i2sd(minute()));
/*  fh.print(",");
  fh.print(avgTemp[0]);
  fh.print(",");
  fh.print(avgTemp[1]);
  fh.print(",");
  fh.print(avgTemp[2]); */
  delay(10);
}
