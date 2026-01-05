#include "extern.h"
unsigned long waitStart, appStart;

 
void ISRwatchDog () {
  watchDog++;
  if (watchDog >= 5) {
    errMess(" watchDog 5s timeout");
    diagMess("resetting master");
    digitalWrite(MASTER_RESET,0);
    delay(10);
    digitalWrite(MASTER_RESET,1);
    ESP.restart();
  }
}

void activity(const char* tag) {
  int ms = millis() - appStart;
  if ( millis() - appStart > 2 ) Serial.printf("%s %dms ",tag,ms);
  appStart = millis();
}

void watchWait(unsigned long timer) {
  waitStart = millis();
  while ( millis()-waitStart < timer) {          // wait for timeout
    unsigned long start = micros();    
    appStart = millis();
    // check for web requests
    server.handleClient();                       // 11ms for metrics
    activity("WEB");
    // check for OTA
    ArduinoOTA.handle();
    activity("OTA");
    // check for FTP request
    FTPcheck = true;
	  ftpSrv.handleFTP();   // 300ms minimum
    FTPcheck = false;                  
    activity("FTP");
    // reset watch dog
    watchDog = 0;
    // statistics
    unsigned long ww = micros() - start;
    if ( ww < WWmin) WWmin = ww;
    if ( ww > WWmax) WWmax = ww;
  }
}
  