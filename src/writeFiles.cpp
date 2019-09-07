#include "extern.h"

byte openFile(const char* s);
void WriteQtr();

byte storeData() {
  strcpy(fileName,todayName);
  if ( !openFile("a") ) return 0;
  WriteQtr();
  fh.close();
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
  fh.print(",");
  fh.print(Vrms);
  fh.print(",");
  fh.println(Wrms[1]);
  delay(10);
}

void updateEnergyFile() {
  fh = SPIFFS.open("Energy.csv", "a");
  if (!fh) {
    diagMess("Energy.csv failed to open");
    return;
  }
  // write daily energy sums
  fh.print(dateStamp());
  for ( int i=1; i<NUM_CHANNELS+1; i++ ) {
    fh.printf(",%6.3f",Energy[i]);
  }
  fh.close();
}