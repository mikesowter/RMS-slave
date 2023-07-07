#include <arduino.h>
#include <ESP8266WiFi.h>
#include <TimeLib.h>
 
#define NUM_CIRCUITS 8
extern float Energy[NUM_CIRCUITS+1], costEnergy[NUM_CIRCUITS+1];
extern float T11_kWh[3];	
extern char longStr[];
void diagMess(const char* mess);
extern float batt_savings[3][3];    // first index is solar, 2nd is battery size
extern float batt_tohouse[3][3], batt_charge[3][3], dump_togrid[3][3];

float readProm(char* unit);
 
void getLastScan() {
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

  T11_kWh[0] = readProm("rmsT11_kWh");
  T11_kWh[1] = readProm("rmsT11_kWh75");
  T11_kWh[2] = readProm("rmsT11_kWh10");
  batt_charge[0][0] = readProm("rmsBatteryCharge");
  batt_tohouse[0][0] = readProm("rmsT11reduction");
  dump_togrid[0][0] = readProm("rmsBatteryOverflow");
  batt_savings[0][0] = readProm("rmsBatterySaving");
  batt_charge[1][0] = readProm("rmsBatteryCharge75");
  batt_tohouse[1][0] = readProm("rmsT11reduction75");
  dump_togrid[1][0] = readProm("rmsBatteryOverflow75");
  batt_charge[2][0] = readProm("rmsBatteryCharge10");
  batt_tohouse[2][0] = readProm("rmsT11reduction10");
  dump_togrid[2][0] = readProm("rmsBatteryOverflow10");

  Serial.printf("query took %li ms\n",millis()-querystart);
}

float readProm(char* unit) {
  #include <ESP8266WiFi.h>
  WiFiClient client;
  char reply[1024];
  uint16_t replyPtr, numPtr;
  uint32_t t = now()-36000;   // back to zulu time

  char host[] = "192.168.1.24";   // RPi-2 prometheus and influx server
  char Str1[] = "GET /api/v1/query_range?query=";
  char Str2[] = "&start=";   
  char Str3[12];
  dtostrf((double)(t-600), 0, 0, Str3);  // start 600s before now
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
        reply[replyPtr++] = client.read();
        if ( replyPtr > 1000 ) {
          diagMess("prometheus reply > 1000 bytes");
          return 0.0;
        }
      }
    }
    if (replyPtr > 20) {
      for (numPtr = replyPtr-8; numPtr>replyPtr-18; numPtr-- ) {
        if (reply[numPtr] == '\"') {
          value = atof(reply+numPtr+1);
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

  
