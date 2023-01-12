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
  Serial.println("reading last values from prometheus");
  WiFiClient client;
  char reply[1024];
  uint16_t replyPtr, cct, numPtr;
  uint32_t t = now()-36000;   // back to zulu time
  uint32_t querystart = millis();
  
  char Str1[] = "GET /api/v1/query_range?query=";
  char Str2[] = "&start=";   // Str2[-1]='0'+cct;
  char Str3[12];
  dtostrf((double)(t-600), 0, 0, Str3);  // start 600s before now
  char Str4[] = "&end=";
  char Str5[12];
  dtostrf((double)t, 0, 0, Str5);
  char Str6[] = "&step=60&timeout=10s HTTP/1.1\r\nHost: ";
  char Str7[] = "\r\nConnection: close\r\n\r\n";
  char query[200];

// read most recent rmsEnergy# values

  strcpy(query,Str1);
  strcat(query,"rmsEnergy#");
  uint8_t cctPtr = strlen(query)-1; // location of cct index in query
  strcat(query,Str2);
  strcat(query,Str3);
  strcat(query,Str4);
  strcat(query,Str5);
  strcat(query,Str6);
  strcat(query,host);
  strcat(query,Str7);
  
  for (cct = 1; cct<9; cct++) {
    query[cctPtr] = '0' + cct;      //rmsEnergy(1->8)
    replyPtr = 0;
    Serial.println(query);
    if (client.connect(host, 9090)) {
      client.write(query,strlen(query));
      while (client.connected() || client.available()) {
        if (client.available()) {
          reply[replyPtr++] = client.read();
          reply[replyPtr] = '\0';
          if ( replyPtr > 1000 ) {
            diagMess("prometheus reply1 > 1000 bytes");
            break;
          }
        }
      }
//      Serial.printf("\n%d last byte: %c\n",replyPtr,reply[replyPtr-1]);
    }
    if (replyPtr > 20) {
      for (numPtr = replyPtr-8; numPtr>replyPtr-18; numPtr-- ) {
        if (reply[numPtr] == '\"') {
          Energy[cct] = atof(reply+numPtr+1);
          Serial.printf("Energy[%d]:%.3f\n\n",cct,Energy[cct]);
          numPtr = 0;
          break;
        }
      }
    }
    client.stop();
    delay(10);
  }  

// read up to 10 most recent rmsCosts#

  strcpy(query,Str1);
  strcat(query,"rmsCost#");
  cctPtr = strlen(query)-1; // location of cct index in query
  strcat(query,Str2);
  strcat(query,Str3);
  strcat(query,Str4);
  strcat(query,Str5);
  strcat(query,Str6);
  strcat(query,host);
  strcat(query,Str7);
  
  for (cct = 1; cct<9; cct++) {
    query[cctPtr] = '0' + cct;      
    replyPtr = 0;
    Serial.println(query);
    if (client.connect(host, 9090)) {
      client.write(query,strlen(query));
      while (client.connected() || client.available()) {
        if (client.available()) {
          reply[replyPtr++] = client.read();
          reply[replyPtr] = '\0';
          if ( replyPtr > 1000 ) {
            diagMess("prometheus reply1 > 1000 bytes");
            break;
          }
        }
      }
//      Serial.printf("\n%d last byte: %c\n",replyPtr,reply[replyPtr-1]);
    }
    if (replyPtr > 20) {
      for (numPtr = replyPtr-8; numPtr>replyPtr-18; numPtr-- ) {
        if (reply[numPtr] == '\"') {
          costEnergy[cct] = atof(reply+numPtr+1);
          Serial.printf("costEnergy[%d]:%.3f\n\n",cct,costEnergy[cct]);
          numPtr = 0;
          break;
        }
      }
    }
    client.stop();
    delay(10);
  } /*
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
*/
  Serial.printf("\n\n query took %li ms\n",millis()-querystart);
}
