#include <extern.h>
extern float T11_kWh[];

float readPromItem(char* unit);
 
void readPromDB() {
  Serial.println("reading last values from prometheus");
  char unit[20];
  
// read most recent rmsEnergy# values
#ifdef RMS1
  for (int cct = 1; cct<9; cct++) {
    strcpy(unit,"rmsEnergy");
    int len = strlen(unit);
    unit[len] = '0' + cct;
    unit[len+1] = '\0';
    Energy[cct] = readPromItem(unit);
  }  

// read most recent rmsEnergyCost# values

  for (int ps = 0; ps<3; ps++) {
    strcpy(unit,"rmsCost00");
    unit[7] = '0' + ps;
    for (int cct = 1; cct<9; cct++) {
      unit[8] = '0' + cct;
      costEnergy[ps][cct] = readPromItem(unit);
    }
  }  
#else
  for (int cct = 0; cct<8; cct++) {
    if ( cct == 1 ) cct = 4;          // jump ccts 1,2,3
    strcpy(unit,"rms2Energy");
    int len = strlen(unit);
    unit[len] = '0' + cct;
    unit[len+1] = '\0';
    Energy[cct] = readPromItem(unit);
  } 
  for (int cct = 4; cct<8; cct++) {
    strcpy(unit,"rms2Pwr_avg");
    int len = strlen(unit);
    unit[len] = '0' + cct;
    unit[len+1] = '\0';
    Wrms_avg[cct] = readPromItem(unit);
  } 
  strcpy(unit,"rms2Imp_meter");
  Imp_meter = readPromItem(unit);
  strcpy(unit,"rms2Exp_meter");
  Exp_meter = readPromItem(unit);
  strcpy(unit,"rms2_15Peak");
  rms15Peak = readPromItem(unit);
  strcpy(unit,"rms2_30Peak");
  rms30Peak = readPromItem(unit);
#endif
// read miscellaneous battery and solar values
#ifdef RMS1
  char root[] = "rms";
  uint8_t po = 10;
  uint8_t bo = 12;

  char promName[25];
  for (uint8_t ps = 0;ps<3;ps++) {
  /*  strcpy(promName,root);
    strcat(promName,"ExcessP0");
    promName[po] = ps + '0';
    T11_Wh[ps] = readPromItem(promName);  */

    for (uint8_t bs = 0;bs<3;bs++) {
      strcpy(promName,root);
      strcat(promName,"ChargeP0B0");
      promName[po] = ps + '0';
      promName[bo] = bs + '0';
      batt_charge[ps][bs] = readPromItem(promName);
    }

    for (uint8_t bs = 0;bs<3;bs++) {
      strcpy(promName,root);
      strcat(promName,"ToHousP0B0");
      promName[po] = ps + '0';
      promName[bo] = bs + '0';
      batt_tohouse[ps][bs] = readPromItem(promName);
    }

    for (uint8_t bs = 0;bs<3;bs++) {
      strcpy(promName,root);
      strcat(promName,"ToGridP0B0");
      promName[po] = ps + '0';
      promName[bo] = bs + '0';
      solar_togrid[ps][bs] = readPromItem(promName);
    }

    for (uint8_t bs = 0;bs<3;bs++) {
      strcpy(promName,root);
      strcat(promName,"SavingP0B0");
      promName[po] = ps + '0';
      promName[bo] = bs + '0';
      batt_savings[ps][bs] = readPromItem(promName);
    }
  }   
  strcpy(promName,"rmsT11_kWh");
  T11_kWh[0] = readPromItem(promName);
  strcpy(promName,"rmsT11_kWh75");
  T11_kWh[1] = readPromItem(promName);
  strcpy(promName,"rmsT11_kWh10");
  T11_kWh[2] = readPromItem(promName);
/*  strcpy(promName,"rms15Peak");
  rms15Peak = readPromItem(promName);
  strcpy(promName,"rms30Peak");
  rms30Peak = readPromItem(promName);
  strcpy(promName,"rms15Demand");
  rms15Demand = readPromItem(promName);
  strcpy(promName,"rms30Demand");
  rms30Demand = readPromItem(promName);  */
#endif
}

float readPromItem(char* unit) {
  #include <ESP8266WiFi.h>
  WiFiClient client;
  uint16_t replyPtr, numPtr;
  unsigned long t = now()-36000;   // back to zulu time
  t -= gobackhrs*3600;        // recover from long outage

  char host[] = "192.168.1.198";   // RPi-2 prometheus and influx server
  char Str1[] = "GET /api/v1/query_range?query=";
  char Str2[] = "&start=";   
  char Str3[12];
  dtostrf((double)(t-1800), 0, 0, Str3);  // start 30m before now
  char Str4[] = "&end=";
  char Str5[12];
  dtostrf((double)t, 0, 0, Str5);
  char Str6[] = "&step=30&timeout=10s HTTP/1.1\r\nHost: ";
  char Str7[] = "\r\nConnection: close\r\n\r\n";
  char query[200];
  float value = 0.0;

  strcpy(query,Str1);
  strcat(query,unit);
  strcat(query,Str2);
  strcat(query,Str3);
  strcat(query,Str4);
  strcat(query,Str5);
  strcat(query,Str6);
  strcat(query,host);
  strcat(query,Str7);

  if (client.connect(host, 9090)) {
    client.write(query,strlen(query));
    replyPtr = 0;
//    Serial.print(query);
    while (client.connected() || client.available()) {
      if (client.available()) {
        longStr[replyPtr++] = client.read();
        if ( replyPtr > LONG_STR_SIZE ) {
          diagMess("prometheus reply > LONG_STR_SIZE");
          return 0.0;
        }
      }
    }
    if (replyPtr > 20) {
      for (numPtr = replyPtr-8; numPtr>replyPtr-18; numPtr-- ) {
        if (longStr[numPtr] == '\"') {
          value = atof(longStr+numPtr+1);
//          Serial.printf("%s:%.3f\n\n",unit,value);
          numPtr = 0;
          break;
        }
      }
    }
  client.stop();
  delay(1);
  }
  return value;
}

  
