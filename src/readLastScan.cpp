#include <arduino.h>
#include <ESP8266WiFi.h>
#include <time.h>
 
char host[] = "192.168.1.20";   // RPi-1 prometheus and influx server
#define NUM_CHANNELS 8
extern float Energy[NUM_CHANNELS+1], costEnergy[NUM_CHANNELS+1];	
void diagMess(const char* mess);
 
void getLastScan() {
  WiFiClient client;
  char buff[1024];
  uint16_t buffPtr, cct, numPtr;
  uint32_t t = now()-36000;   // back to zulu time
  
  char Str1a[] = "GET /api/v1/query_range?query=rmsEnergy";
  char Str1b[] = "GET /api/v1/query_range?query=rmsCost";
  char Str2[] = "1&start=";   // Str2[0]='0'+cct;
  char Str3[12];
  dtostrf((double)(t-300), 0, 0, Str3);
  char Str4[] = "&end=";
  char Str5[12];
  dtostrf((double)t, 0, 0, Str5);
  char Str6[] = "&step=60&timeout=10s HTTP/1.1\r\nHost: ";
  char Str7[] = "\r\nConnection: close\r\n\r\n";
// read up to 5 prior energies
  for (cct = 1; cct<9; cct++) {
    strcpy(buff,Str1a);
    strcat(buff,Str2);
    strcat(buff,Str3);
    strcat(buff,Str4);
    strcat(buff,Str5);
    strcat(buff,Str6);
    strcat(buff,host);
    strcat(buff,Str7);

    buff[strlen(Str1a)] = '0' + cct;      //rmsEnergy(1->8)
    if (client.connect(host, 9090)) {
      client.write(buff,strlen(buff));
      buffPtr = 0;
      while (client.connected() || client.available()) {
        if (client.available()) {
          buff[buffPtr++] = client.read();
          if ( buffPtr > 1000 ) {
            diagMess("prometheus reply > 1000 bytes");
            break;
          }
        }
      }
      buff[buffPtr] = '\0';
      Serial.printf("\n%d bytes: \n%s\n",buffPtr,buff);
      for (numPtr = buffPtr-8; numPtr>buffPtr-18; numPtr-- ) {
        if (buff[numPtr] == '\"') {
          Energy[cct] = atof(buff+numPtr+1);
        }
      }
      client.stop();
      delay(100);
    }
  }
// read up to 5 prior costs
  for (cct = 1; cct<9; cct++) {
    strcpy(buff,Str1b);
    strcat(buff,Str2);
    strcat(buff,Str3);
    strcat(buff,Str4);
    strcat(buff,Str5);
    strcat(buff,Str6);
    strcat(buff,host);
    strcat(buff,Str7);

    buff[strlen(Str1b)] = '0' + cct;      //rmsCost(1->8)
    if (client.connect(host, 9090)) {
      client.write(buff,strlen(buff));
      buffPtr = 0;
      while (client.connected() || client.available()) {
        if (client.available()) {
          buff[buffPtr++] = client.read();
          if ( buffPtr > 1000 ) {
            diagMess("prometheus reply > 1000 bytes");
            break;
          }
        }
      }
      buff[buffPtr] = '\0';
      Serial.printf("\n%d bytes: \n%s\n",buffPtr,buff);
      for (numPtr = buffPtr-8; numPtr>buffPtr-18; numPtr-- ) {
        if (buff[numPtr] == '\"') {
          costEnergy[cct] = atof(buff+numPtr+1);
        }
      }
      client.stop();
      delay(100);
    }
  }
}