#include "extern.h"

void handleRoot() {
  longStr[0]='\0';
  addCstring("\n# TYPE rmsVolts guage" );
  addCstring("\nrmsVolts ");
  addCstring(f2s2(Vrms));
  addCstring("\n# TYPE rmsVmin guage" );
  addCstring("\nrmsVmin ");
  addCstring(f2s2(Vmin));
  addCstring("\n# TYPE rmsVmax guage" );
  addCstring("\nrmsVmax ");
  addCstring(f2s2(Vmax));
  addCstring("\n# TYPE rmsEnergy1 guage" );
  addCstring("\nrmsEnergy1 ");
  addCstring(f2s2(Energy[1]));
  addCstring("\n# TYPE rmsFreq guage" );
  addCstring("\nrmsFreq ");
  addCstring(f2s4(Freq));
  addCstring("\n# TYPE rmsWifiSignal guage" );
  addCstring("\nrmsWifiSignal ");
  addCstring(f2s2(-WiFi.RSSI()));
  addCstring( "\n" );
  server.send ( 200, "text/plain", longStr );
}

void handleMetrics() {
  longStr[0]='\0';
  addCstring("\n# TYPE rmsVolts guage" );
  addCstring("\nrmsVolts ");
  addCstring(f2s2(Vrms));
  addCstring("\n# TYPE rmsVmin guage" );
  addCstring("\nrmsVmin ");
  addCstring(f2s2(Vmin));
  addCstring("\n# TYPE rmsVmax guage" );
  addCstring("\nrmsVmax ");
  addCstring(f2s2(Vmax));
  addCstring("\n# TYPE rmsFreq guage" );
  addCstring("\nrmsFreq ");
  addCstring(f2s4(Freq));
  addCstring("\n# TYPE rmsPwr_min1 guage" );
  addCstring("\nrmsPwr_min1 ");
  addCstring(f2s2(Wrms_min[1]));
  addCstring("\n# TYPE rmsPwr_min2 guage" );
  addCstring("\nrmsPwr_min2 ");
  addCstring(f2s2(Wrms_min[2]));
  addCstring("\n# TYPE rmsPwr_min3 guage" );
  addCstring("\nrmsPwr_min3 ");
  addCstring(f2s2(Wrms_min[3]));
  addCstring("\n# TYPE rmsPwr_min4 guage" );
  addCstring("\nrmsPwr_min4 ");
  addCstring(f2s2(Wrms_min[4]));
  addCstring("\n# TYPE rmsPwr_min5 guage" );
  addCstring("\nrmsPwr_min5 ");
  addCstring(f2s2(Wrms_min[5]));
  addCstring("\n# TYPE rmsPwr_min6 guage" );
  addCstring("\nrmsPwr_min6 ");
  addCstring(f2s2(Wrms_min[6]));
  addCstring("\n# TYPE rmsPwr_min7 guage" );
  addCstring("\nrmsPwr_min7 ");
  addCstring(f2s2(Wrms_min[7]));
  addCstring("\n# TYPE rmsPwr_min8 guage" );
  addCstring("\nrmsPwr_min8 ");
  addCstring(f2s2(Wrms_min[8]));

  addCstring("\n# TYPE rmsPwr_max1 guage" );
  addCstring("\nrmsPwr_max1 ");
  addCstring(f2s2(Wrms_max[1]));
  addCstring("\n# TYPE rmsPwr_max2 guage" );
  addCstring("\nrmsPwr_max2 ");
  addCstring(f2s2(Wrms_max[2]));
  addCstring("\n# TYPE rmsPwr_max3 guage" );
  addCstring("\nrmsPwr_max3 ");
  addCstring(f2s2(Wrms_max[3]));
  addCstring("\n# TYPE rmsPwr_max4 guage" );
  addCstring("\nrmsPwr_max4 ");
  addCstring(f2s2(Wrms_max[4]));
  addCstring("\n# TYPE rmsPwr_max5 guage" );
  addCstring("\nrmsPwr_max5 ");
  addCstring(f2s2(Wrms_max[5]));
  addCstring("\n# TYPE rmsPwr_max6 guage" );
  addCstring("\nrmsPwr_max6 ");
  addCstring(f2s2(Wrms_max[6]));
  addCstring("\n# TYPE rmsPwr_max7 guage" );
  addCstring("\nrmsPwr_max7 ");
  addCstring(f2s2(Wrms_max[7]));
  addCstring("\n# TYPE rmsPwr_max8 guage" );
  addCstring("\nrmsPwr_max8 ");
  addCstring(f2s2(Wrms_max[8]));

  addCstring("\n# TYPE rmsEnergy1 guage" );
  addCstring("\nrmsEnergy1 ");
  addCstring(f2s4(Energy[1]));
  addCstring("\n# TYPE rmsEnergy2 guage" );
  addCstring("\nrmsEnergy2 ");
  addCstring(f2s4(Energy[2]));
  addCstring("\n# TYPE rmsEnergy3 guage" );
  addCstring("\nrmsEnergy3 ");
  addCstring(f2s4(Energy[3]));
  addCstring("\n# TYPE rmsEnergy4 guage" );
  addCstring("\nrmsEnergy4 ");
  addCstring(f2s4(Energy[4]));
  addCstring("\n# TYPE rmsEnergy5 guage" );
  addCstring("\nrmsEnergy5 ");
  addCstring(f2s4(Energy[5]));
  addCstring("\n# TYPE rmsEnergy6 guage" );
  addCstring("\nrmsEnergy6 ");
  addCstring(f2s4(Energy[6]));
  addCstring("\n# TYPE rmsEnergy7 guage" );
  addCstring("\nrmsEnergy7 ");
  addCstring(f2s4(Energy[7]));
  addCstring("\n# TYPE rmsEnergy8 guage" );
  addCstring("\nrmsEnergy8 ");
  addCstring(f2s4(Energy[8]));
 
  addCstring("\n# TYPE rmsWifiSignal guage" );
  addCstring("\nrmsWifiSignal ");
  addCstring(f2s2(-WiFi.RSSI()));
  addCstring( "\n" );
  server.send ( 200, "text/plain", longStr );
  // reset max,min for scan interval
  Vmin = 500.0;
  Vmax = 0.0;
  Vrms_min = 500.0;
  Vrms_max = 0.0;
  for (int i=1;i<9;i++) {
    Wrms_min[i] = 5000.0;
    Wrms_max[i] = 0.0;
  }
  Serial.print("  scanned");
  lastScan = millis();
 }

void handleWater() {
  if ( waterOn ) server.send ( 200, "text/plain", "On" );
  else server.send ( 200, "text/plain", "Off" );
 };

void handleNotFound() {
  server.uri().toCharArray(userText, 14);
  Serial.println(userText);
  if (strncmp(userText,"/reset",6)==0) {
    errMess("User requested restart");
    updateEnergyFile();
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
  else if (strncmp(userText,"/delerrs",8)==0) {
    SPIFFS.remove("/errmess.txt");
    fe = SPIFFS.open("/errmess.txt", "a");
    dateStamp();
    strcpy(charBuf,"<!DOCTYPE html><html><head><HR>Errors deleted<HR></head></html>");
    server.send ( 200, "text/html", charBuf );
  }
  else if (SPIFFS.exists(userText)) {
    strcpy(longStr,"File: ");
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
    server.send ( 200, "text/plain", longStr );
  }
  else if (strncmp(userText,"/favicon.ico",12)==0) {
  }
  else if (strncmp(userText,"/apple",6)==0) {
  }
  else {
    strcpy(charBuf,userText);
    strcat(charBuf," is not a valid option");
    errMess(charBuf);
    helpPage();
  }
}

void addCstring(const char* s) {
  // find end of longStr
  uint16_t p;
  for (p=0;p<longStrSize;p++) {
    if ( p>longStrSize-32) {
      diagMess("longStrSize exceeded");
      break;
    }
    if (longStr[p]=='\0') {
      break;    // p now points to end of old string
    }
  }
  uint16_t q=0;
  for (;p<longStrSize;p++) {
    longStr[p]=s[q];
//    if (s[q]!='\0') Serial.print(s[q]);
    if (s[q++]=='\0') break;
  }
  htmlLen = p;
}
