#include "extern.h"
uint32_t waitStart, appStart;

 
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

void watchWait(uint32_t timer) {
  waitStart = millis();
  while ( millis()-waitStart < timer) {          // wait for timeout
    uint32_t start = micros();    
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
	  ftpSrv.handleFTP(FS_ID);                     // 300ms minimum
    activity("FTP");
    // reset watch dog
    watchDog = 0;
    // statistics
    uint32_t ww = micros() - start;
    if ( ww < WWmin) WWmin = ww;
    if ( ww > WWmax) WWmax = ww;
  }
}
  