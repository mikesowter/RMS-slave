 #include "extern.h"
 
 void ISRwatchDog () {
  watchDog++;
  if (watchDog == 10) {
    diagMess("watchDog 10s alert");
  }
  else if (watchDog >= 30) {
    errMess("watchDog 30s");
    delay(500);
    fd.close();
    fe.close();
    ESP.restart();
  }
}

void watchWait(uint32_t timer) {
  uint32_t waitStart = millis();
  while (millis()-waitStart < timer) {  // wait for timeout
    if (waitStart > millis()) waitStart = millis(); // check for wrap around
    delay(10);
    //  check for web requests
    server.handleClient();
    // check for OTA
    ArduinoOTA.handle();
    // check for FTP request
		// ftpSrv.handleFTP();
    // reset watch dog
    watchDog = 0;
  }
}
  