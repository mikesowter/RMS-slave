#include "extern.h"
char promName[15];
extern float T11_kWh[], T11_inc[];
extern float batt_tohouse[3][3], dump_togrid[3][3], batt_charge[3][3];
extern uint8_t gobackhrs;

void promform(const char* pname,float lval, uint8_t res) {
  char fltStr[12];
  dtostrf((double)lval, 0, res, fltStr);
  sprintf(charBuf,"\n# TYPE %s guage\n%s %s",pname, pname, fltStr);
  strcat(longStr,charBuf);
}

void handleRoot() {
  longStr[0]='\0';
  strcat(longStr,"\nVolts ");
  strcat(longStr,f2s2(Vrms));
  strcat(longStr,"\nUsed_kWh ");
  strcat(longStr,f2s4(Energy[1]));
  strcat(longStr,"\nT11_kWh ");
  strcat(longStr,f2s4(T11_kWh[0]));
  strcat(longStr,"\nT31_kWh ");
  strcat(longStr,f2s4(Energy[5]));
  strcat(longStr,"\nsolar_kWh ");
  strcat(longStr,f2s4(Energy[7]));
  strcat(longStr,"\nWifiSignal ");
  strcat(longStr,f2s2(-WiFi.RSSI()));
  server.send ( 200, "text/plain", longStr );
}


void handleMetrics() {
  longStr[0] = '\0';
#ifdef RMS8
  promform("rms8Vbattery", Vbat, 2);
  promform("rms8WifiSignal", -WiFi.RSSI(), 0);
  promform("rms8Volts", Vrms, 2);
  promform("rms8Vmin", Vrms_min, 2);
  promform("rms8Vmax", Vrms_max, 2);
  promform("rms8Vpmin_p", Vmin_p, 2);
  promform("rms8Vpmax_p", Vmax_p, 2);
  promform("rms8Vpmin_n", Vmin_n, 2);
  promform("rms8Vpmax_n", Vmax_n, 2);
  promform("rms8Freq", Freq, 4);
#else
  promform("rmsVbattery", Vbat, 2);
  promform("rmsWifiSignal", -WiFi.RSSI(), 0);
  promform("rmsVolts", Vrms, 2);
  promform("rmsVmin", Vrms_min, 2);
  promform("rmsVmax", Vrms_max, 2);
  promform("rmsVpmin_p", Vmin_p, 2);
  promform("rmsVpmax_p", Vmax_p, 2);
  promform("rmsVpmin_n", Vmin_n, 2);
  promform("rmsVpmax_n", Vmax_n, 2);
  promform("rmsFreq", Freq, 4);
#endif

  if ( !pwrOutage && Wrms_min[1] != 9999.0F ) {
 #ifdef RMS8
 // power
    promform("rms8Pwr_min1", Wrms_min[1], 2);
    promform("rms8Pwr_min2", Wrms_min[2], 2);
    promform("rms8Pwr_min3", Wrms_min[3], 2);
    promform("rms8Pwr_min4", Wrms_min[4], 2);
    promform("rms8Pwr_min5", Wrms_min[5], 2);
    promform("rms8Pwr_min6", Wrms_min[6], 2);
    promform("rms8Pwr_min7", Wrms_min[7], 2);
    promform("rms8Pwr_min8", Wrms_min[8], 2);

    promform("rms8Pwr_max1", Wrms_max[1], 2);
    promform("rms8Pwr_max2", Wrms_max[2], 2);
    promform("rms8Pwr_max3", Wrms_max[3], 2);
    promform("rms8Pwr_max4", Wrms_max[4], 2);
    promform("rms8Pwr_max5", Wrms_max[5], 2);
    promform("rms8Pwr_max6", Wrms_max[6], 2);
    promform("rms8Pwr_max7", Wrms_max[7], 2);
    promform("rms8Pwr_max8", Wrms_max[8], 2);
// energy
    promform("rms8Energy1", Energy[1], 2);
    promform("rms8Energy2", Energy[2], 2);
    promform("rms8Energy3", Energy[3], 2);
    promform("rms8Energy4", Energy[4], 2);
    promform("rms8Energy5", Energy[5], 2);
    promform("rms8Energy6", Energy[6], 2);
    promform("rms8Energy7", Energy[7], 2);
    promform("rms8Energy8", Energy[8], 2);
#else
// power
    promform("rmsPwr_min1", Wrms_min[1], 2);
    promform("rmsPwr_min2", Wrms_min[2], 2);
    promform("rmsPwr_min3", Wrms_min[3], 2);
    promform("rmsPwr_min4", Wrms_min[4], 2);
    promform("rmsPwr_min5", Wrms_min[5], 2);
    promform("rmsPwr_min6", Wrms_min[6], 2);
    promform("rmsPwr_min7", Wrms_min[7], 2);
    promform("rmsPwr_min8", Wrms_min[8], 2);

    promform("rmsPwr_max1", Wrms_max[1], 2);
    promform("rmsPwr_max2", Wrms_max[2], 2);
    promform("rmsPwr_max3", Wrms_max[3], 2);
    promform("rmsPwr_max4", Wrms_max[4], 2);
    promform("rmsPwr_max5", Wrms_max[5], 2);
    promform("rmsPwr_max6", Wrms_max[6], 2);
    promform("rmsPwr_max7", Wrms_max[7], 2);
    promform("rmsPwr_max8", Wrms_max[8], 2);
// energy
    promform("rmsEnergy1", Energy[1], 2);
    promform("rmsEnergy2", Energy[2], 2);
    promform("rmsEnergy3", Energy[3], 2);
    promform("rmsEnergy4", Energy[4], 2);
    promform("rmsEnergy5", Energy[5], 2);
    promform("rmsEnergy6", Energy[6], 2);
    promform("rmsEnergy7", Energy[7], 2);
    promform("rmsEnergy8", Energy[8], 2);

    promform("rmsT11_kWh", T11_kWh[0], 4);
    promform("rmsT11_kWh75", T11_kWh[1], 4);
    promform("rmsT11_kWh10", T11_kWh[2], 4);
#endif
#ifdef RMS8
// costs
    promform("rms8Cost1", costEnergy[1], 2);
    promform("rms8Cost2", costEnergy[2], 2);
    promform("rms8Cost3", costEnergy[3], 2);
    promform("rms8Cost4", costEnergy[4], 2);
    promform("rms8Cost5", costEnergy[5], 2);
    promform("rms8Cost6", costEnergy[6], 2);
    promform("rms8Cost7", costEnergy[7], 2);
    promform("rms8Cost8", costEnergy[8], 2);
#else 
// costs
    promform("rmsCost1", costEnergy[1], 2);
    promform("rmsCost2", costEnergy[2], 2);
    promform("rmsCost3", costEnergy[3], 2);
    promform("rmsCost4", costEnergy[4], 2);
    promform("rmsCost5", costEnergy[5], 2);
    promform("rmsCost6", costEnergy[6], 2);
    promform("rmsCost7", costEnergy[7], 2);
    promform("rmsCost8", costEnergy[8], 2);

// battery simulation
    for (uint8_t ps = 0;ps<3;ps++) {
      strcpy(promName,"rmsExcessP0 ");
      promName[10] = ps + '0';
      promform(promName,excessSolar[ps],5);

      for (uint8_t bs = 0;bs<3;bs++) {
        strcpy(promName,"rmsChargeP0B0 ");
        promName[10] = ps + '0';
        promName[12] = bs + '0';
        promform(promName,batt_charge[ps][bs],2);
      }

      for (uint8_t bs = 0;bs<3;bs++) {
        strcpy(promName,"rmsToGridP0B0 ");
        promName[10] = ps + '0';
        promName[12] = bs + '0';
        promform(promName,dump_togrid[ps][bs],2);
      }

      for (uint8_t bs = 0;bs<3;bs++) {
        strcpy(promName,"rmsToHousP0B0 ");
        promName[10] = ps + '0';
        promName[12] = bs + '0';
        promform(promName,batt_tohouse[ps][bs],2);
      }

      for (uint8_t bs = 0;bs<3;bs++) {
        strcpy(promName,"rmsSavingP0B0 ");
        promName[10] = ps + '0';
        promName[12] = bs + '0';
        promform(promName,batt_savings[ps][bs],2);
      }
    }
    promform("rmsSpareSolar", avSparekW, 2);
  }
#endif
#ifdef RMS8
    // housekeeping
    promform("rms8WfdTimeMin", (float)WFDmin, 0);
    promform("rms8WfdTimeMax", (float)WFDmax, 0);
    promform("rms8WaitWatchMin", (float)WWmin, 0);
    promform("rms8WaitWatchMax", (float)WWmax, 0);
    promform("rms8MissedCycles", (float)missedCycle, 0);
#else
    // housekeeping
    promform("rmsWfdTimeMin", (float)WFDmin, 0);
    promform("rmsWfdTimeMax", (float)WFDmax, 0);
    promform("rmsWaitWatchMin", (float)WWmin, 0);
    promform("rmsWaitWatchMax", (float)WWmax, 0);
    promform("rmsMissedCycles", (float)missedCycle, 0);
#endif

  server.send ( 200, "text/plain", longStr );
  // reset max,min for prometheus scan interval
  Vmin_n = 500.0F;
  Vmax_n = 0.0F;
  Vmin_p = 500.0F;
  Vmax_p = 0.0F;
  Vrms_min = 500.0F;
  Vrms_max = 0.0F;
  for (int i=1;i<9;i++) {
    Wrms_min[i] = 9999.0F;
    Wrms_max[i] = 0.0F;
  }
  noDataYet = true;
  lastScan = millis();
  scanSec = second();
  WWmin = 9999;
  WWmax = 0;
  LTmin = 9999;
  LTmax = 0;
  WFDmin = 9999;
  WFDmax = 0;
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
    errMess("User requested reset");
    ESP.restart();
  }
  else if (strncmp(userText,"/shutdown",9)==0) {
    errMess("User requested shutdown");
//    uploadDay();
    strcpy(charBuf,"<!DOCTYPE html><html><head><HR>Safe to Shutdown<HR></head></html>");
    server.send ( 200, "text/html", charBuf );
  }
  else if (strncmp(userText,"/deldiags",9)==0) {
    LittleFS.remove("/diags.txt");
    fd = LittleFS.open("/diags.txt", "a");
    diagMess("diags deleted");
    fd.close();
    strcpy(charBuf,"<!DOCTYPE html><html><head><HR>Diags deleted<HR></head></html>");
    server.send ( 200, "text/html", charBuf );
  }
  else if (strncmp(userText,"/delerrs",8)==0) {
    LittleFS.remove("/errmess.txt");
    fe = LittleFS.open("/errmess.txt", "a");
    errMess("errors deleted");
    fe.close();
    strcpy(charBuf,"<!DOCTYPE html><html><head><HR>Errors deleted<HR></head></html>");
    server.send ( 200, "text/html", charBuf );
  }
  else if (LittleFS.exists(userText)) {
    strcpy(longStr,"File: ");
    strcat(longStr,userText);
    strcat(longStr,"\r\r");
    fh = LittleFS.open(userText, "r");
    if ( fh.size() > 2000 ) fd.seek(-2000,SeekEnd);
    while (fh.available()) {
      int k=fh.readBytesUntil('\r',charBuf,160);
      charBuf[k]='\0';
      strcat(longStr,charBuf);
    }
    fh.close();
    server.send ( 200, "text/plain", longStr );
  }
  else if (strncmp(userText,"/favicon.ico",12)==0) {
  }
  else if (strncmp(userText,"/apple",6)==0) {
  }
  else if (strncmp(userText,"/spareSol",9)==0) {
    strcpy(longStr,"\n# TYPE rmsSpareSolar guage");
    strcat(longStr,"\nrmsSpareSolar ");
    strcat(longStr,f2s2(avSparekW));
    strcat(longStr,"\n" );
    server.send ( 200, "text/plain", longStr ); 
  }
  else if (strncmp(userText,"/goback24",9)==0) {
    gobackhrs = 24;
    getLastScan();
    gobackhrs = 0;
    errMess("data recovered from yesterday");
    strcpy(charBuf,"<!DOCTYPE html><html><head><HR>back 24hrs<HR></head></html>");
    server.send ( 200, "text/html", charBuf );
  }
  else if (strncmp(userText,"/goback48",9)==0) {
    gobackhrs = 48;
    getLastScan();
    gobackhrs = 0;
    errMess("data recovered from 2 days back");
    strcpy(charBuf,"<!DOCTYPE html><html><head><HR>back 48hrs<HR></head></html>");
    server.send ( 200, "text/html", charBuf );
  }
  else {
    strcpy(charBuf,userText);
    strcat(charBuf," is not a valid option");
    errMess(charBuf);
    helpPage();
  }
}

