#include <arduino.h>
#include <ESP8266WiFi.h>
#include <time.h>
 
char host[] = "192.168.1.24";   // RPi-2 prometheus and influx server
#define NUM_CIRCUITS 8
extern float Energy[NUM_CIRCUITS+1], costEnergy[NUM_CIRCUITS+1], T11_kWh;	
extern char longStr[];
void diagMess(const char* mess);
extern float batt_charge;
extern float batt_tohouse, batt_togrid;
extern float batt_savings, batt_costs;
 
void getLastScan() {
  Serial.println("reading last good RMS energy and cost values");
  WiFiClient client;
  char buff[1024];
  uint16_t buffPtr, cct, numPtr;
  uint32_t t = now()-36000;   // back to zulu time
  uint32_t querystart = millis();
  
  char Str1[] = "GET /api/v1/query_range?query=";
  char Str2[] = "&start=";   // Str2[-1]='0'+cct;
  char Str3[12];
  dtostrf((double)(t-600), 0, 0, Str3);
  char Str4[] = "&end=";
  char Str5[12];
  dtostrf((double)t, 0, 0, Str5);
  char Str6[] = "&step=60&timeout=10s HTTP/1.1\r\nHost: ";
  char Str7[] = "\r\nConnection: close\r\n\r\n";

// read most recent rmsEnergy[8] values

  strcpy(buff,Str1);
  strcat(buff,"rmsEnergy ");
  uint8_t cctPtr = strlen(buff); // point to location of cct index
  strcat(buff,Str2);
  strcat(buff,Str3);
  strcat(buff,Str4);
  strcat(buff,Str5);
  strcat(buff,Str6);
  strcat(buff,host);
  strcat(buff,Str7);

  for (cct = 1; cct<9; cct++) {
    buff[cctPtr] = '0' + cct;      //rmsEnergy(1->8)
    if (client.connect(host, 9090)) {
      client.write(buff,strlen(buff));
      buffPtr = 0;
      while (client.connected() || client.available()) {
        if (client.available()) {
          buff[buffPtr++] = client.read();
          if ( buffPtr > 1000 ) {
            diagMess("prometheus reply1 > 1000 bytes");
            break;
          }
        }
      }
      buff[buffPtr] = '\0';
//      sprintf(longStr,"\n%d bytes: \n%s\n",buffPtr,buff);
      for (numPtr = buffPtr-8; numPtr>buffPtr-18; numPtr-- ) {
        if (buff[numPtr] == '\"') {
          Energy[cct] = atof(buff+numPtr+1);
        }
      }
      client.stop();
      delay(100);
    }
  }
// read up to 5 most recent rmsCosts[]

  strcpy(buff,Str1);
  strcat(buff,"rmsCost ");
  cctPtr = strlen(buff);      // point to location of cct index
  strcat(buff,Str2);
  strcat(buff,Str3);
  strcat(buff,Str4);
  strcat(buff,Str5);
  strcat(buff,Str6);
  strcat(buff,host);
  strcat(buff,Str7);

  for (cct = 1; cct<9; cct++) {
    buff[cctPtr] = '0' + cct;      //rmsCost(1->8)
    if (client.connect(host, 9090)) {
      client.write(buff,strlen(buff));
      buffPtr = 0;
      while (client.connected() || client.available()) {
        if (client.available()) {
          buff[buffPtr++] = client.read();
          if ( buffPtr > 1000 ) {
            diagMess("prometheus reply2 > 1000 bytes");
            break;
          }
        }
      }
      buff[buffPtr] = '\0';
//      sprintf(longStr,"\n%d bytes: \n%s\n",buffPtr,buff);
      for (numPtr = buffPtr-8; numPtr>buffPtr-18; numPtr-- ) {
        if (buff[numPtr] == '\"') {
          costEnergy[cct] = atof(buff+numPtr+1);
        }
      }
      client.stop();
      delay(100);
    }
  }
  // read up to 5 most recent T11 energy estimate

  strcpy(buff,Str1);
  strcat(buff,"rmsT11_kWh");
  strcat(buff,Str2);
  strcat(buff,Str3);
  strcat(buff,Str4);
  strcat(buff,Str5);
  strcat(buff,Str6);
  strcat(buff,host);
  strcat(buff,Str7);

  if (client.connect(host, 9090)) {
    client.write(buff,strlen(buff));
    buffPtr = 0;
    while (client.connected() || client.available()) {
      if (client.available()) {
        buff[buffPtr++] = client.read();
        if ( buffPtr > 1000 ) {
          diagMess("prometheus reply3 > 1000 bytes");
          break;
        }
      }
    }
    buff[buffPtr] = '\0';
    Serial.printf("\n%d bytes: \n%s\n",buffPtr,buff);
    for (numPtr = buffPtr-8; numPtr>buffPtr-18; numPtr-- ) {
      if (buff[numPtr] == '\"') {
        T11_kWh = atof(buff+numPtr+1);
      }
    }
    client.stop();
    delay(100);
  }

  // read up to 5 most recent rmsBatteryCharge

  strcpy(buff,Str1);
  strcat(buff,"rmsBatteryCharge");
  strcat(buff,Str2);
  strcat(buff,Str3);
  strcat(buff,Str4);
  strcat(buff,Str5);
  strcat(buff,Str6);
  strcat(buff,host);
  strcat(buff,Str7);

  if (client.connect(host, 9090)) {
    client.write(buff,strlen(buff));
    buffPtr = 0;
    while (client.connected() || client.available()) {
      if (client.available()) {
        buff[buffPtr++] = client.read();
        if ( buffPtr > 1000 ) {
          diagMess("prometheus reply4 > 1000 bytes");
          break;
        }
      }
    }
    buff[buffPtr] = '\0';
    Serial.printf("\n%d bytes: \n%s\n",buffPtr,buff);
    for (numPtr = buffPtr-8; numPtr>buffPtr-18; numPtr-- ) {
      if (buff[numPtr] == '\"') {
        batt_charge = atof(buff+numPtr+1);
      }
    }
    client.stop();
    delay(100);
  }

  // read up to 5 most recent rmsBatteryOverflow

  strcpy(buff,Str1);
  strcat(buff,"rmsBatteryOverflow");
  strcat(buff,Str2);
  strcat(buff,Str3);
  strcat(buff,Str4);
  strcat(buff,Str5);
  strcat(buff,Str6);
  strcat(buff,host);
  strcat(buff,Str7);

  if (client.connect(host, 9090)) {
    client.write(buff,strlen(buff));
    buffPtr = 0;
    while (client.connected() || client.available()) {
      if (client.available()) {
        buff[buffPtr++] = client.read();
        if ( buffPtr > 1000 ) {
          diagMess("prometheus reply5 > 1000 bytes");
          break;
        }
      }
    }
    buff[buffPtr] = '\0';
    Serial.printf("\n%d bytes: \n%s\n",buffPtr,buff);
    for (numPtr = buffPtr-8; numPtr>buffPtr-18; numPtr-- ) {
      if (buff[numPtr] == '\"') {
        batt_togrid = atof(buff+numPtr+1);
      }
    }
    client.stop();
    delay(100);
  }


  // read up to 5 most recent rmsT11reduction

  strcpy(buff,Str1);
  strcat(buff,"rmsT11reduction");
  strcat(buff,Str2);
  strcat(buff,Str3);
  strcat(buff,Str4);
  strcat(buff,Str5);
  strcat(buff,Str6);
  strcat(buff,host);
  strcat(buff,Str7);

  if (client.connect(host, 9090)) {
    client.write(buff,strlen(buff));
    buffPtr = 0;
    while (client.connected() || client.available()) {
      if (client.available()) {
        buff[buffPtr++] = client.read();
        if ( buffPtr > 1000 ) {
          diagMess("prometheus reply6 > 1000 bytes");
          break;
        }
      }
    }
    buff[buffPtr] = '\0';
    Serial.printf("\n%d bytes: \n%s\n",buffPtr,buff);
    for (numPtr = buffPtr-8; numPtr>buffPtr-18; numPtr-- ) {
      if (buff[numPtr] == '\"') {
        batt_tohouse = atof(buff+numPtr+1);
      }
    }
    client.stop();
    delay(100);
  }

  Serial.printf("\n\n query took %li ms\n",millis()-querystart);
}
