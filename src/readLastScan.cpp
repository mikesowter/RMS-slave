#include <extern.h>

float readProm(char* unit);
 
void readPromDB() {
  Serial.println("reading last values from prometheus");
  uint32_t querystart = millis();
  char unit[20];
  
// read most recent rmsEnergy# values

  for (int cct = 1; cct<9; cct++) {
    strcpy(unit,"rmsEnergy");
    int len = strlen(unit);
    unit[len] = '0' + cct;
    unit[len+1] = '\0';
    Energy[cct] = readProm(unit);
  }  

// read most recent rmsEnergyCost# values

  for (int cct = 1; cct<9; cct++) {
    strcpy(unit,"rmsCost");
    int len = strlen(unit);
    unit[len] = '0' + cct;
    unit[len+1] = '\0';
    costEnergy[cct] = readProm(unit);
  }  

// read miscellaneous recent values
#ifdef RMS2
  char root[] = "rms2";
  uint8_t po = 11;
  uint8_t bo = 13;
#else 
  char root[] = "rms";
  uint8_t po = 10;
  uint8_t bo = 12;

  char promName[20];
  for (uint8_t ps = 0;ps<3;ps++) {
    strcpy(promName,root);
    strcat(promName,"ExcessP0 ");
    promName[po] = ps + '0';
    T11_kWh[ps] = readProm(promName);

    for (uint8_t bs = 0;bs<3;bs++) {
      strcpy(promName,root);
      strcat(promName,"ChargeP0B0 ");
      promName[po] = ps + '0';
      promName[bo] = bs + '0';
      batt_charge[ps][bs] = readProm(promName);
    }

    for (uint8_t bs = 0;bs<3;bs++) {
      strcpy(promName,root);
      strcat(promName,"ToHousP0B0 ");
      promName[po] = ps + '0';
      promName[bo] = bs + '0';
      batt_tohouse[ps][bs] = readProm(promName);
    }

    for (uint8_t bs = 0;bs<3;bs++) {
      strcpy(promName,root);
      strcat(promName,"ToGridP0B0 ");
      promName[po] = ps + '0';
      promName[bo] = bs + '0';
      dump_togrid[ps][bs] = readProm(promName);
    }

    for (uint8_t bs = 0;bs<3;bs++) {
      strcpy(promName,root);
      strcat(promName,"SavingP0B0 ");
      promName[po] = ps + '0';
      promName[bo] = bs + '0';
      batt_savings[ps][bs] = readProm(promName);
    }
  }   
  #endif

  Serial.printf("query took %li ms\n",millis()-querystart);
}

float readProm(char* unit) {
  #include <ESP8266WiFi.h>
  WiFiClient client;
  uint16_t replyPtr, numPtr;
  uint32_t t = now()-36000;   // back to zulu time
  t -= gobackhrs*3600;        // recover from long outage

  char host[] = "192.168.1.24";   // RPi-2 prometheus and influx server
  char Str1[] = "GET /api/v1/query_range?query=";
  char Str2[] = "&start=";   
  char Str3[12];
  dtostrf((double)(t-1800), 0, 0, Str3);  // start 30m before now
  char Str4[] = "&end=";
  char Str5[12];
  dtostrf((double)t, 0, 0, Str5);
  char Str6[] = "&step=60&timeout=10s HTTP/1.1\r\nHost: ";
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
    Serial.print(query);
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
          Serial.printf("%s:%.3f\n\n",unit,value);
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

  
