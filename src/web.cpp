#include "extern.h"
char promName[15];
extern float T11_kWh[], T11_inc[];
extern float batt_tohouse[3][3], dump_togrid[3][3], batt_charge[3][3];
extern uint8_t gobackhrs;

void promform(const char* pname,float lval, uint8_t res) {
  char fltStr[12];
  dtostrf((double)lval, 0, res, fltStr);
  if ( newArg ) {
    sprintf(charBuf,"\n# TYPE %s gauge\n%s %s",pname, pname, fltStr);
    newArg = false;
  }
  sprintf(charBuf,"\n%s %s", pname, fltStr);
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
  activity("root ");
}


void handleMetrics() {
  if (FTPcheck) return;
  longStr[0] = '\0';
#ifdef RMS2
  promform("rms2Vbattery", Vbat, 2);
  promform("rms2WifiSignal", -WiFi.RSSI(), 0);
  promform("rms2Volts", Vrms, 2);
  promform("rms2Vmin", Vrms_min, 2);
  promform("rms2Vmax", Vrms_max, 2);
  promform("rms2Vmin_p", Vmin_p, 1);
  promform("rms2Vmax_p", Vmax_p, 1);
  promform("rms2Vmin_n", Vmin_n, 1);
  promform("rms2Vmax_n", Vmax_n, 1);
  if ( Freq > 45.0 ) promform("rms2Freq", Freq, 3);
#else
  promform("rmsVbattery", Vbat, 2);
  promform("rmsWifiSignal", -WiFi.RSSI(), 0);
  promform("rmsVolts", Vrms, 2);
  promform("rmsVpmax_p", Vmax_p, 2);
  promform("rmsVpmin_n", Vmin_n, 2);
  promform("rmsFreq", Freq, 3);
#endif

  if ( !pwrOutage && Wrms_min[1] != 9999.0F ) {
 #ifdef RMS2
 // power
 //   promform("rms2Pwr_min1", Wrms_min[1], 2);
 //   promform("rms2Pwr_min2", Wrms_min[2], 2);
 //   promform("rms2Pwr_min3", Wrms_min[3], 2);
    promform("rms2Pwr_min4", Wrms_min[4], 2);
    promform("rms2Pwr_min5", Wrms_min[5], 2);
    promform("rms2Pwr_min6", Wrms_min[6], 2);
    promform("rms2Pwr_min7", Wrms_min[7], 2);

//    promform("rms2Pwr_max1", Wrms_max[1], 2);
//    promform("rms2Pwr_max2", Wrms_max[2], 2);
//    promform("rms2Pwr_max3", Wrms_max[3], 2);
    promform("rms2Pwr_max4", Wrms_max[4], 2);
    promform("rms2Pwr_max5", Wrms_max[5], 2);
    promform("rms2Pwr_max6", Wrms_max[6], 2);
    promform("rms2Pwr_max7", Wrms_max[7], 2);

    promform("rms2Pwr_avg4", Wrms_avg[4], 2);
    promform("rms2Pwr_avg5", Wrms_avg[5], 2);
    promform("rms2Pwr_avg6", Wrms_avg[6], 2);
    promform("rms2Pwr_avg7", Wrms_avg[7], 2);

// energy
    promform("rms2Energy0", Energy[0], 3);
    promform("rms2Energy4", Energy[4], 3);
    promform("rms2Energy5", Energy[5], 3);
    promform("rms2Energy6", Energy[6], 3);
    promform("rms2Energy7", Energy[7], 3);
    promform("rms2Imp_meter",Imp_meter,3);
    promform("rms2Exp_meter",Exp_meter,3);

// interval power import / export
    promform("rms2_15Peak", rms15Peak, 3);
    promform("rms2_30Peak", rms30Peak, 3);
    promform("rms2_5Demand", rms5Demand, 3);
    promform("rms2_15Demand", rms15Demand, 3);
    promform("rms2_30Demand", rms30Demand, 3);
//    promform("rms2_5FeedIn", FI_5m_kW, 3);
//    promform("rms2_15FeedIn", FI_15m_kW, 3);
//    promform("rms2_30FeedIn", FI_30m_kW, 3);
    
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
    
    promform("rmsT11_kW", T11_W, 3);
    promform("rms15Peak", rms15Peak, 3);
    promform("rms30Peak", rms30Peak, 3);
    promform("rms15Demand", rms15Demand, 3);
    promform("rms30Demand", rms30Demand, 3);
// energy
    promform("rmsEnergy1", Energy[1], 3);
    promform("rmsEnergy2", Energy[2], 3);
    promform("rmsEnergy3", Energy[3], 3);
    promform("rmsEnergy4", Energy[4], 3);
    promform("rmsEnergy5", Energy[5], 3);
    promform("rmsEnergy6", Energy[6], 3);
    promform("rmsEnergy7", Energy[7], 3);
    promform("rmsEnergy8", Energy[8], 3);

    promform("rmsT11_kWh", T11_kWh[0], 3);
    promform("rmsT11_kWh75", T11_kWh[1], 3);
    promform("rmsT11_kWh10", T11_kWh[2], 3);
    
#endif
#ifdef RMS1
// costs
    for (uint8_t ps = 0;ps<3;ps++) {
      for (uint8_t cct = 1;cct<9;cct++) {
        strcpy(promName,"rmsCost00");
        promName[7] = ps + '0';
        promName[8] = cct + '0';
        promform(promName, costEnergy[ps][cct], 3);
      }
    }

// battery simulation
    for (uint8_t ps = 0;ps<3;ps++) {
      strcpy(promName,"rmsExcessP0");
      promName[10] = ps + '0';
      promform(promName,excessSolar[ps],5);

      for (uint8_t bs = 0;bs<3;bs++) {
        strcpy(promName,"rmsChargeP0B0");
        promName[10] = ps + '0';
        promName[12] = bs + '0';
        promform(promName,batt_charge[ps][bs],2);
      }

      for (uint8_t bs = 0;bs<3;bs++) {
        strcpy(promName,"rmsToGridP0B0");
        promName[10] = ps + '0';
        promName[12] = bs + '0';
        promform(promName,dump_togrid[ps][bs],3);
      }

      for (uint8_t bs = 0;bs<3;bs++) {
        strcpy(promName,"rmsToHousP0B0");
        promName[10] = ps + '0';
        promName[12] = bs + '0';
        promform(promName,batt_tohouse[ps][bs],2);
      }

      for (uint8_t bs = 0;bs<3;bs++) {
        strcpy(promName,"rmsSavingP0B0");
        promName[10] = ps + '0';
        promName[12] = bs + '0';
        promform(promName,batt_savings[ps][bs],2);
      }
    }
    strcpy(promName,"rmsSpareSolar");
    promform(promName, avSparekW, 3);
#endif
  }

#ifdef RMS2
  // housekeeping
  promform("rms2WfdTimeMin", (float)WFDmin, 0);
  promform("rms2WfdTimeMax", (float)WFDmax, 0);
  promform("rms2WaitWatchMin", (float)WWmin, 0);
  promform("rms2WaitWatchMax", (float)WWmax, 0);
  promform("rms2MissedCycles", (float)missedCycle, 0);
  promform("rms2ScanTime_min", (float)t_scan_min, 0);
  promform("rms2ScanTime_max", (float)t_scan_max, 0);
  promform("rms2watchDog", (float)watchDog, 0);
#else
  // housekeeping
  promform("rmsWfdTimeMin", (float)WFDmin, 0);
  promform("rmsWfdTimeMax", (float)WFDmax, 0);
  promform("rmsWaitWatchMin", (float)WWmin, 0);
  promform("rmsWaitWatchMax", (float)WWmax, 0);
  promform("rmsMissedCycles", (float)missedCycle, 0);
  promform("rmsScanTime_min", (float)t_scan_min, 0);
  promform("rmsScanTime_max", (float)t_scan_max, 0);
#endif

  server.send ( 200, "text/plain", longStr );
  // reset max,min for prometheus scan interval
  Vmin_n = 500.0F;
  Vmax_n = 0.0F;
  Vmin_p = 500.0F;
  Vmax_p = 0.0F;
  Vrms_min = 500.0F;
  Vrms_max = 0.0F;
  for (int i=1;i<=NUM_CCTS;i++) {
    Wrms_min[i] = 9999.0F;
    Wrms_max[i] = -9999.0F;
  }
  noDataYet = true;
  lastScan = millis();
  scanSec = second();
  t_scan_max = 0UL;
  t_scan_min = 9999UL;
  WWmin = 9999;
  WWmax = 0;
  LTmin = 9999;
  LTmax = 0;
  WFDmin = 9999;
  WFDmax = 0;
  activity("mets");
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
  activity("hotw ");
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
    activity("deld");
  }
  else if (strncmp(userText,"/delerrs",8)==0) {
    LittleFS.remove("/errmess.txt");
    fe = LittleFS.open("/errmess.txt", "a");
    errMess("errors deleted");
    fe.close();
    strcpy(charBuf,"<!DOCTYPE html><html><head><HR>Errors deleted<HR></head></html>");
    server.send ( 200, "text/html", charBuf );
    activity("dele");
  }
  else if (LittleFS.exists(userText)) {
    strcpy(longStr,"File: ");
    strcat(longStr,userText);
    strcat(longStr,"\r\r");
    fh = LittleFS.open(userText, "r");
    if ( fh.size() > 5000 ) {
      strcat(longStr,"file exceeds 5000 chars\r");
    }
    else {
      while (fh.available()) {
        int k=fh.readBytesUntil('\r',charBuf,160);
        charBuf[k]='\0';
        strcat(longStr,charBuf);
      }
    }
    fh.close();
    server.send ( 200, "text/plain", longStr );
    activity("file");
  }
  else if (strncmp(userText,"/favicon.ico",12)==0) {
    activity("favi");
  }
  else if (strncmp(userText,"/apple",6)==0) {
    activity("appl");
  }
  else if (strncmp(userText,"/spareSol",9)==0) {
    strcpy(longStr,"\nrmsSpareSolar ");
    strcat(longStr,f2s2(avSparekW));
    server.send ( 200, "text/plain", longStr );
    activity("spar");
  }
  else if (strncmp(userText,"/goback",7)==0) {
    gobackhrs = atoi(userText+8);
    readPromDB();
    gobackhrs = 0;
    strcpy(charBuf,"data recovered from ");
    strcat(charBuf,userText+8);
    strcat(charBuf," hrs back");
    errMess(charBuf);
    server.send ( 200, "text/plain", charBuf );
    activity("gobk");
  }
  else if (strncmp(userText,"/pause",4)==0) {
    strcpy(charBuf," stating 30s pause ");
    server.send ( 200, "text/plain", charBuf );
    unsigned long pauseStart=millis();
    do {
      // check for OTA & FTP
      ArduinoOTA.handle();
      ftpSrv.handleFTP(FS_ID);
      watchDog = 0;
      delay(1);
    } while ( millis() - pauseStart < 30000);
  }
  else if (strncmp(userText,"/update",7)==0) {
    char* tok;
    tok = strtok(userText,",");
    tok = strtok(NULL,",");
    Imp_meter = atof(tok);
    tok = strtok(NULL,",");
    Exp_meter = atof(tok);
    sprintf(charBuf,"Tariff11: %.3f, FI: %.3f\n",Imp_meter,Exp_meter);
    diagMess(charBuf);  
    strcpy(charBuf,"<!DOCTYPE html><html><head><HR>Tariff11 and FI updated<HR></head></html>");
    server.send ( 200, "text/html", charBuf );
  }
  else {
    strcpy(charBuf,userText);
    strcat(charBuf," is not a valid option");
    errMess(charBuf);
    helpPage();
    activity("help");
  }
}

