#include "extern.h"
unsigned long waitStart, appStart;

 
void ISRwatchDog () {
  watchDog++;
  if (watchDog >= 20) {
    errMess("watchDog 20s timeout");
    ESP.restart();
  }
}

void activity(const char* tag) {
  int ms = millis() - appStart;
  if ( millis() - appStart > 1 ) Serial.printf("%s %dms ",tag,ms);
  appStart = millis();
}

void watchWait(unsigned long timer) {
  waitStart = millis();
  while ( millis()-waitStart < timer) {          // wait for timeout
    unsigned long start = micros();    
    // check for hotwater query
    if ( udp.parsePacket() ) handleWater();      // 600us max
    appStart = millis();
    // check for web requests
    server.handleClient();                       // 16ms for metrics
    activity("SVR");
    // check for OTA
    ArduinoOTA.handle();
    activity("OTA");
    // check for FTP request
    FTPcheck = true;
	  ftpSrv.handleFTP(FS_ID);   // 300ms minimum
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
  