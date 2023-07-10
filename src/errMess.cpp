#include <extern.h>

/*-------- display code ----------*/

void errMess(const char* mess) {
  fe = LittleFS.open("/diags.txt","a");
  Serial.println(mess);
  fe.print(dateStamp());
  fe.print(" ");
  fe.print(timeStamp());
  fe.println(mess);
  fe.close();
}

void diagMess(const char* mess) {
  fd = LittleFS.open("/diags.txt","a");
  Serial.println(mess);
  fd.print(dateStamp());
  fd.print(" ");
  fd.print(timeStamp());
  fd.println(mess);
  fd.close();
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

// convert float into char string ddddd.dddd
char* f2s5(float f) {
  dtostrf((double)f, 0, 5, fltStr);
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

// convert float fraction into an 6 char string (.)dddddd
char* f2s6(float f) {
  byte ptr = 0;
  uint32_t rank = 100000;         //six places right of dp
  uint32_t right = (uint32_t)(1000000.0*f);
  for ( uint8_t n=0; n<6; n++ ) {
    d8Str[ptr++] = right/rank +'0';
    right %= rank;
    rank /= 10;
  }
  d8Str[ptr]='\0';
  return d8Str;
}