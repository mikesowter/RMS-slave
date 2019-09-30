#include "extern.h"
 
 void ISRwatchDog () {
  watchDog++;
  if (watchDog == 10) {
    diagMess("watchDog 10s alert");
  }
  else if (watchDog >= 30) {
    errMess("watchDog 30s timeout");
    updateEnergyFile();
    delay(10);
    fd.close();
    fe.close();
    ESP.restart();
  }
  if ( millis() - lastScan == 90000UL ) {
    diagMess("no scan for 90s");
    // rejoin local network if necessary
	  if (WiFi.status() != WL_CONNECTED) joinNet();
  }
}

void watchWait(uint32_t timer) {
  uint32_t waitStart = millis();
  while (millis()-waitStart < timer) {  // wait for timeout
    if (waitStart > millis()) waitStart = millis(); // check for wrap around
    // check for hotwater query
    if ( udp.parsePacket() ) handleWater();
    // check for web requests
    server.handleClient();
    // check for OTA
    ArduinoOTA.handle();
    // check for FTP request
	  ftpSrv.handleFTP();
    // reset watch dog
    watchDog = 0;
  }
}
  