#include <extern.h>
#include <ESP8266WiFi.h>
extern WiFiClient client;
uint16_t replyPtr, numPtr;
float value;

void fillBarrel() {
  char unit[20] = "rms2Imp_meter";
  
// read most recent meter values at 5min intervals

// "GET /api/v1/query_range?query=rms2Imp_meter&start=ssssssssssssss&end=eeeeeeeeeeee
//  &step=300&timeout=10s HTTP/1.1\r\nHost: "192.168.1.198"\r\nConnection: close\r\n\r\n"

  unsigned long t = now()-36000;   // back to zulu time

  char host[] = "192.168.1.198";   // RPi-2 prometheus and influx server
  char Str1[] = "GET /api/v1/query_range?query=";
  char Str2[] = "&start=";   
  char Str3[12];
  dtostrf((double)(t-1800), 0, 0, Str3);  // start 30m before now
  char Str4[] = "&end=";
  char Str5[12];
  dtostrf((double)t, 0, 0, Str5);
  char Str6[] = "&step=300&timeout=10s HTTP/1.1\r\nHost: ";
  char Str7[] = "\r\nConnection: close\r\n\r\n";
  char query[200];

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
          diagMess("Barrel reply > LONG_STR_SIZE");
        }
      }
    }
    longStr[replyPtr] = '\0';
    fh = LittleFS.open("BarrelData.txt","a+");


/*  typical reply:
{"status":"success","data":{"resultType":"matrix","result":[{"metric":{"__name__":"rms2Imp_meter","instance":"192.168.1.62:80","job":"RMS2"},
"values":[[1729492439,"51.363"],[1729492739,"51.419"],[1729493039,"51.475"],[1729493339,"51.605"],[1729493639,"51.788"],[1729493939,"51.967"]]}]}}
*/

    if (replyPtr > 20) {
        char* tok; 
        tok = strstr(longStr,"values");
        fh.printf("\n%s",tok);
        uint8_t en_index = (minute()/5)%6; 
        for ( int i=0;i<6;i++ ) {
            tok = strstr(tok+1,",\"");
            value = atof(tok+2);
            fh.printf(" %d: %0.3f",en_index,value);
            en_index = (en_index+1)%6; 
            Imp_5m_kWh[en_index] = value;
        }
    }
    fh.close();
    client.stop();
    delay(1);
  }
}


  
