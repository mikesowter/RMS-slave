#include "extern.h"

void handleRoot() {
  longStr[0]='\0';
  addCstring("\nVolts ");
  addCstring(f2s2(Vrms));
  addCstring("\nUsed_kWh ");
  addCstring(f2s4(Energy[1]));
  addCstring("\nT11_kWh ");
  addCstring(f2s4(T11_kWh));
  addCstring("\nT31_kWh ");
  addCstring(f2s4(Energy[5]));
  addCstring("\nsolar_kWh ");
  addCstring(f2s4(Energy[7]));
  addCstring("\nWifiSignal ");
  addCstring(f2s2(-WiFi.RSSI()));

  server.send ( 200, "text/plain", longStr );
}

void handleMetrics() {
  longStr[0]='\0';
  addCstring("\n# TYPE rmsVbattery guage" );
  addCstring("\nrmsVbattery ");
  addCstring(f2s2(Vbat));
  addCstring("\n# TYPE rmsWifiSignal guage" );
  addCstring("\nrmsWifiSignal ");
  addCstring(f2s2(-WiFi.RSSI()));
  if ( !pwrOutage ) {
    addCstring("\n# TYPE rmsVolts guage" );
    addCstring("\nrmsVolts ");
    addCstring(f2s2(Vrms));
    addCstring("\n# TYPE rmsVmin guage" );
    addCstring("\nrmsVmin ");
    addCstring(f2s2(Vrms_min));
    addCstring("\n# TYPE rmsVmax guage" );
    addCstring("\nrmsVmax ");
    addCstring(f2s2(Vrms_max));
    addCstring("\n# TYPE rmsVpmin_p guage" );
    addCstring("\nrmsVpmin_p ");
    addCstring(f2s2(Vmin_p));
    addCstring("\n# TYPE rmsVpmax_p guage" );
    addCstring("\nrmsVpmax_p ");
    addCstring(f2s2(Vmax_n));
    addCstring("\n# TYPE rmsVpmin_n guage" );
    addCstring("\nrmsVpmin_n ");
    addCstring(f2s2(Vmin_n));
    addCstring("\n# TYPE rmsVpmax_n guage" );
    addCstring("\nrmsVpmax_n ");
    addCstring(f2s2(Vmax_n));
    addCstring("\n# TYPE rmsFreq guage" );
    addCstring("\nrmsFreq ");
    addCstring(f2s4(Freq));

    addCstring("\n# TYPE rmsPwr_min1 guage" ); //total import
    addCstring("\nrmsPwr_min1 ");
    addCstring(f2s2(Wrms_min[1]));
    addCstring("\n# TYPE rmsPwr_min2 guage" ); //cct1 bedrooms 1&2
    addCstring("\nrmsPwr_min2 ");
    addCstring(f2s2(Wrms_min[2]));
    addCstring("\n# TYPE rmsPwr_min3 guage" ); //cct2 kitchen lounge
    addCstring("\nrmsPwr_min3 ");
    addCstring(f2s2(Wrms_min[3]));
    addCstring("\n# TYPE rmsPwr_min4 guage" ); //cct3 downstairs
    addCstring("\nrmsPwr_min4 ");
    addCstring(f2s2(Wrms_min[4]));
    addCstring("\n# TYPE rmsPwr_min5 guage" ); //hotwater
    addCstring("\nrmsPwr_min5 ");
    addCstring(f2s2(Wrms_min[5]));
    addCstring("\n# TYPE rmsPwr_min6 guage" ); //oven
    addCstring("\nrmsPwr_min6 ");
    addCstring(f2s2(Wrms_min[6]));
    addCstring("\n# TYPE rmsPwr_min7 guage" ); //solar
    addCstring("\nrmsPwr_min7 ");
    addCstring(f2s2(Wrms_min[7]));
    addCstring("\n# TYPE rmsPwr_min8 guage" ); //lights
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
  // energy
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
    addCstring("\n# TYPE rmsT11_kWh guage" );
    addCstring("\nrmsT11_kWh ");
    addCstring(f2s4(T11_kWh));
  // costs
    addCstring("\n# TYPE rmsCost1 guage" ); //heat pumps
    addCstring("\nrmsCost1 ");
    addCstring(f2s2(costEnergy[1]));
    addCstring("\n# TYPE rmsCost2 guage" );
    addCstring("\nrmsCost2 ");
    addCstring(f2s2(costEnergy[2]));
    addCstring("\n# TYPE rmsCost3 guage" );
    addCstring("\nrmsCost3 ");
    addCstring(f2s2(costEnergy[3]));
    addCstring("\n# TYPE rmsCost4 guage" );
    addCstring("\nrmsCost4 ");
    addCstring(f2s2(costEnergy[4]));
    addCstring("\n# TYPE rmsCost5 guage" );
    addCstring("\nrmsCost5 ");
    addCstring(f2s2(costEnergy[5]));
    addCstring("\n# TYPE rmsCost6 guage" );
    addCstring("\nrmsCost6 ");
    addCstring(f2s2(costEnergy[6]));
    addCstring("\n# TYPE rmsCost7 guage" ); //FIT
    addCstring("\nrmsCost7 ");
    addCstring(f2s4(costEnergy[7]));
    addCstring("\n# TYPE rmsCost8 guage" );
    addCstring("\nrmsCost8 ");
    addCstring(f2s2(costEnergy[8]));
  // battery simulation
    extern float batt_tohouse, batt_togrid, batt_charge;
    addCstring("\n# TYPE rmsBatteryFlow guage" );
    addCstring("\nrmsBatteryFlow ");
    addCstring(f2s4(solar-loads));
    addCstring("\n# TYPE rmsBatteryCharge guage" );
    addCstring("\nrmsBatteryCharge ");
    addCstring(f2s2(batt_charge));
    addCstring("\n# TYPE rmsBatteryOverflow guage" );
    addCstring("\nrmsBatteryOverflow ");
    addCstring(f2s2(batt_togrid));
    addCstring("\n# TYPE rmsT11reduction guage" );
    addCstring("\nrmsT11reduction ");
    addCstring(f2s2(batt_tohouse));
    addCstring("\n# TYPE rmsBatterySaving guage" );
    addCstring("\nrmsBatterySaving ");
    addCstring(f2s2(batt_savings - batt_costs));
    addCstring("\n# TYPE rmsSpareSolar guage" );
    addCstring("\nrmsSpareSolar ");
    addCstring(f2s2(avSparekW));
  /* loop analysis
    addCstring("\n# TYPE rmsWaiting guage" );
    addCstring("\nrmsWaiting ");
    addCstring(f2s2((float)waiting));
    addCstring("\n# TYPE rmsScanSec guage" );
    addCstring("\nrmsScanSec ");
    addCstring(f2s2((float)scanSec));
    addCstring("\n# TYPE rmsWWmin guage" );
    addCstring("\nrmsWWmin ");
    addCstring(f2s2((float)WWmin));
    addCstring("\n# TYPE rmsWWmax guage" );
    addCstring("\nrmsWWmax ");
    addCstring(f2s2((float)WWmax));
    addCstring("\n# TYPE rmsWDmin guage" );
    addCstring("\nrmsWDmin ");
    addCstring(f2s2((float)WDmin));
    addCstring("\n# TYPE rmsWDmax guage" );
    addCstring("\nrmsWDmax ");
    addCstring(f2s2((float)WDmax)); */
  }
  addCstring( "\n" );
  server.send ( 200, "text/plain", longStr );
  // reset max,min for scan interval
  Vmin_n = 500.0;
  Vmax_n = 0.0;
  Vmin_p = 500.0;
  Vmax_p = 0.0;
  Vrms_min = 500.0;
  Vrms_max = 0.0;
  for (int i=1;i<9;i++) {
    Wrms_min[i] = 4999.0;
    Wrms_max[i] = 0.0;
  }
  lastScan = millis();
  scanSec = second();
  WWmin = 9999;
  WWmax = 0;
  WDmin = 9999;
  WDmax = 0;
 }

void handleWater() {
  char incomingPacket[32], replyPacket[9];
  if ( waterOn ) strcpy(replyPacket,"It's on ");
  else strcpy(replyPacket,"It's off");
  // receive incoming UDP packets
  int len = udp.read(incomingPacket, 32);
  if (len > 0) incomingPacket[len] = 0;
  // send back a reply, to the IP address and port we got the packet from
  udp.beginPacket(udp.remoteIP(), udp.remotePort());
  udp.write(replyPacket);
  udp.endPacket();
 };

void handleNotFound() {
  server.uri().toCharArray(userText, 30);
//  Serial.println(userText);
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
    fd.close();
    LittleFS.remove("/diags.txt");
    fd = LittleFS.open("/diags.txt", "a");
    diagMess("diags deleted");
    fd.flush();
    strcpy(charBuf,"<!DOCTYPE html><html><head><HR>Diags deleted<HR></head></html>");
    server.send ( 200, "text/html", charBuf );
  }
  else if (strncmp(userText,"/delerrs",8)==0) {
    LittleFS.remove("/errmess.txt");
    fe = LittleFS.open("/errmess.txt", "a");
    errMess("errors deleted");
    fe.flush();
    strcpy(charBuf,"<!DOCTYPE html><html><head><HR>Errors deleted<HR></head></html>");
    server.send ( 200, "text/html", charBuf );
  }
  else if (LittleFS.exists(userText)) {
    
    strcpy(longStr,"File: ");
    addCstring(userText);
    addCstring("\r\r");
    fh = LittleFS.open(userText, "r");
  //  if ( fh.size() > 2000 ) fd.seek(-2000,SeekEnd);
    while (fh.available()) {
      int k=fh.readBytesUntil('\r',charBuf,160);
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
  else if (strncmp(userText,"/battCGH",8)==0) {
    char* tok;
    tok = strtok(userText,",");
    tok = strtok(NULL,",");
    batt_charge = (float)atof(tok);
    tok = strtok(NULL,",");
    batt_togrid = (float)atof(tok);
    tok = strtok(NULL," ");
    batt_tohouse = (float)atof(tok);
    sprintf(charBuf,"sim battery charge: %s",f2s2(batt_charge));
    diagMess(charBuf);  
    strcpy(charBuf,"<!DOCTYPE html><html><head><HR>battery data updated<HR></head></html>");
    server.send ( 200, "text/html", charBuf );
  }
   else if (strncmp(userText,"/spareSol",9)==0) {
  /*  C[0]='\0';
    addCstring("\n# TYPE rmsSpareSolar guage" );
    addCstring("\nrmsSpareSolar ");
    addCstring(f2s2(avSparekW));
    addCstring( "\n" );
    server.send ( 200, "text/plain", longStr ); */
    strcpy(longStr,"\n# TYPE rmsSpareSolar guage");
    strcat(longStr,"\nrmsSpareSolar ");
    strcat(longStr,f2s2(avSparekW));
    strcat(longStr,"\n" );
    server.send ( 200, "text/plain", longStr ); 
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
  for (p=0; p<longStrSize; p++) {
    if ( p > longStrSize-32) {
      diagMess("longStrSize exceeded");
      break;
    }
    if ( longStr[p] == '\0' ) {
      break;    // p now points to end of old string
    }
  }
  uint16_t q = 0;
  for ( ; p<longStrSize; p++ ) {
    longStr[p] = s[q];
    if ( s[q++] == '\0' ) break;
  }
  htmlLen = p;
} 
/*  why doesnt this work?
void addCstring(const char* s) {
  if ( CstringPtr > longStrSize-strlen(s) ) {
    diagMess("longStrSize exceeded");
    return;
  }  
  for ( uint16_t q = 0; strlen(s); q++ ) {
    longStr[CstringPtr++] = s[q];
  }
  longStr[CstringPtr] = '\0'; 
***********************************************************  or just: 
  strcat( longStr, s );        // is it leaky? 
  */
