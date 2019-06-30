#include "extern.h"

void handleMetrics() {
  htmlStr[0]='\0';
  addCstring("\n# TYPE rmsVolts guage" );
  addCstring("\nrmsVolts ");
  addCstring(f2s2(Vrms));
  addCstring("\n# TYPE rmsVmin guage" );
  addCstring("\nrmsVmin -");
  addCstring(f2s2(Vmin));
  addCstring("\n# TYPE rmsVmax guage" );
  addCstring("\nrmsVmax ");
  addCstring(f2s2(Vmax));
  addCstring("\n# TYPE rmsFreq guage" );
  addCstring("\nrmsFreq ");
  addCstring(f2s4(Freq));
  addCstring("\n# TYPE rmsAmps1 guage" );
  addCstring("\nrmsAmps1 ");
  addCstring(f2s4(Irms[1]));
  addCstring("\n# TYPE rmsPwr1 guage" );
  addCstring("\nrmsPwr1 ");
  addCstring(f2s2(Wrms[1]));
  addCstring("\n# TYPE rmsWifiSignal guage" );
  addCstring("\nrmsWifiSignal ");
  addCstring(f2s2(-WiFi.RSSI()));
  addCstring( "\n" );
  server.send ( 200, "text/plain", htmlStr );
  Vmin = 500.0;
  Vmax = 0.0;
  Serial.println("    scanned");
 }

void handleNotFound() {
  server.uri().toCharArray(userText, 14);
  Serial.println(userText);
  if (strncmp(userText,"/reset",6)==0) {
    errMess("User requested restart");
    fd.close();
    fe.close();
    ESP.restart();
  }
  else if (strncmp(userText,"/shutdown",9)==0) {
    errMess("User requested shutdown");
//    uploadDay();
    strcpy(charBuf,"<!DOCTYPE html><html><head><HR>Safe to Shutdown<HR></head></html>");
    server.send ( 200, "text/html", charBuf );
  }
  else if (strncmp(userText,"/deldiags",9)==0) {
    SPIFFS.remove("/diags.txt");
    fd = SPIFFS.open("/diags.txt", "a");
    dateStamp();
    strcpy(charBuf,"<!DOCTYPE html><html><head><HR>Diags deleted<HR></head></html>");
    server.send ( 200, "text/html", charBuf );
  }
  else if (strncmp(userText,"/delerrs",9)==0) {
    SPIFFS.remove("/errmess.txt");
    fe = SPIFFS.open("/errmess.txt", "a");
    dateStamp();
    strcpy(charBuf,"<!DOCTYPE html><html><head><HR>Errors deleted<HR></head></html>");
    server.send ( 200, "text/html", charBuf );
  }
  else if (SPIFFS.exists(userText)) {
    strcpy(htmlStr,"File: ");
    addCstring(userText);
    addCstring("\r\r");
    fh = SPIFFS.open(userText, "r");

    while (fh.available()) {
      int k=fh.readBytesUntil('\r',charBuf,80);
      charBuf[k]='\0';
      addCstring(charBuf);
      delay(10);
    }
    fh.close();
    server.send ( 200, "text/plain", htmlStr );
  }
  else if (strncmp(userText,"/favicon.ico",12)==0) {
  }
  else if (strncmp(userText,"/apple",6)==0) {
  }
  else if (strncmp(userText,"/dir",4)==0) {
    dirList();
  }
  else if (strncmp(userText,"/diags",6)==0) {
    listDiags();
  }
  else {
    strcpy(charBuf,userText);
    strcat(charBuf," is not a valid option");
    errMess(charBuf);
    helpPage();
  }
}

void addCstring(const char* s) {
  // find end of htmlStr
  uint16_t p;
  for (p=0;p<HTML_SIZE;p++) {
    if ( p>HTML_SIZE-32) {
      diagMess("HTML_SIZE exceeded");
      break;
    }
    if (htmlStr[p]=='\0') {
      break;    // p now points to end of old string
    }
  }
  uint16_t q=0;
  for (;p<HTML_SIZE;p++) {
    htmlStr[p]=s[q];
//    if (s[q]!='\0') Serial.print(s[q]);
    if (s[q++]=='\0') break;
  }
  htmlLen = p;
}
