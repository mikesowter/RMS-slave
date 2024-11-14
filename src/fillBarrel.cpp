#include <extern.h>
#include <ESP8266WiFi.h>

uint16_t replyPtr, numPtr;
float value;

void fillBarrel() {
// read most recent meter values at 5min intervals

  t1 = millis();
  unsigned long t = now() - 36000;        // Zulu time used in database
  unsigned long next = 300UL - t%300UL;   // when demand calc will next run 
  Serial.printf("\nsecs to next 5min: %lu\n",next);
  
  char unit[20] = "rms2Imp_meter";
  char host[] = "192.168.1.198";          // RPi-2 prometheus and influx server
  char Str1[] = "GET /api/v1/query_range?query=";
  char Str2[] = "&start=";   
  char Str3[12];
  dtostrf((double)(t-1800UL), 0, 0, Str3);  // start 30m before next run
  char Str4[] = "&end=";
  char Str5[12];
  dtostrf((double)(t-300UL), 0, 0, Str5);
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

  Serial.print(query);

  if (client.connect(host, 9090)) {
    client.write(query,strlen(query));
    replyPtr = 0;
    while (client.connected() || client.available()) {
      if (client.available()) {
        longStr[replyPtr++] = client.read();
        if ( replyPtr > LONG_STR_SIZE ) {
          diagMess("Barrel reply > LONG_STR_SIZE");
        }
      }
    }
    longStr[replyPtr] = '\0';
    Serial.println(longStr);
    


/*  typical reply:
{"status":"success","data":{"resultType":"matrix","result":[{"metric":{"__name__":"rms2Imp_meter","instance":"192.168.1.62:80","job":"RMS2"},
"values":[[1729492439,"51.363"],[1729492739,"51.419"],[1729493039,"51.475"],[1729493339,"51.605"],[1729493639,"51.788"],[1729493939,"51.967"]]}]}}
    empty reply:
{"status":"success","data":{"resultType":"matrix","result":[]}}       */

    if (replyPtr > 65) {
        char* tok; 
        tok = strstr(longStr,"values");
        fh = LittleFS.open("BarrelData.txt","a+");
        fh.printf("\n%s %s ",dateStamp(),timeStamp());
        uint8_t en_index = (minute()/5)%6;  // points to times 5 minutes ago
        for ( int i=0;i<6;i++ ) {
            tok = strstr(tok+1,",\"");
            value = atof(tok+2);
            fh.printf(" %d: %0.3f",en_index,value);
            en_index = (en_index+1)%6; 
            Imp_5m_kWh[en_index] = value;
        }
        fh.printf("  now: %0.3f",Imp_meter);
        fh.close();
    }
    else {
      diagMess(" fillBarrel query failed");
      Serial.println(" fillBarrel query failed");
      for ( int i=0;i<6;i++ ) Imp_5m_kWh[i] = Imp_meter;
    }
    client.stop();
    delay(1);
  }
  Serial.printf("\nsecs elapsed for query %lu\n", millis()-t1);
}


  
