#include "C:\Users\Mikes 6700K\Dropbox\sketchbook\RMS slave\src\extern.h"

/*-------- display code ----------*/

void errMess(const char* mess) {
  fe.print(dateStamp());
  fe.print(" ");
  fe.print(timeStamp());
  fe.println(mess);
}

void diagMess(const char* mess) {
  fd.print(dateStamp());
  fd.print(" ");
  fd.print(timeStamp());
  fd.println(mess);
}

char* dateStamp() {
  // digital display of the date
  strcpy(dateStr,i2sd(year()%100));
  strcat(dateStr,i2sd(month()));
  strcat(dateStr,i2sd(day()));
  return dateStr;
}

char* timeStamp() {
  // digital display of the time
  strcpy(timeStr,i2sd(hour()));
  strcat(timeStr,":");
  strcat(timeStr,i2sd(minute()));
  strcat(timeStr,":");
  strcat(timeStr,i2sd(second()));
  strcat(timeStr," ");
  return timeStr;
}

// convert integer into a 2 char string dd
char* i2sd(uint8_t b) {
  d2Str[0] = b/10+'0';
  d2Str[1] = b%10+'0';
  return d2Str;
}

// convert float into char string ddddd.dd
char* f2s2(float f) {
  dtostrf((double)f, 0, 2, fltStr);
  return fltStr;
}

// convert float into char string ddddd.dddd
char* f2s4(float f) {
  dtostrf((double)f, 0, 4, fltStr);
  return fltStr;
}

// convert integer into a 2 Hex string dd
char* i2sh(uint8_t b) {
  int hi=b/16;
  if (hi>9) d2Str[0] = hi +'A' -10;
  else d2Str[0] = hi +'0';
  int lo=b%16;
  if (lo>9) d2Str[1] = lo +'A' -10;
  else d2Str[1] = lo +'0';
  return d2Str;
}