#include "extern.h"
 
 void ISRwatchDog () {
  watchDog++;
  if (watchDog >= 20) {
    errMess("watchDog 20s timeout");
    ESP.restart();
  }
}

void watchWait(uint32_t timer) {
  uint32_t waitStart = millis();
  while ( millis()-waitStart < timer) {          // wait for timeout
    uint32_t start = micros();    
    // check for hotwater query
    if ( udp.parsePacket() ) handleWater();      // 600us max
    // check for web requests
    server.handleClient();                       // 40ms for metrics
    // check for OTA
    ArduinoOTA.handle();
    // check for FTP request
	  ftpSrv.handleFTP(FS_ID);                     // 300ms minimum
    // reset watch dog
    watchDog = 0;
    // do background
    yield();
    // statistics
    uint32_t ww = micros() - start;
    if ( ww < WWmin) WWmin = ww;
    if ( ww > WWmax) WWmax = ww;
  }
}
  