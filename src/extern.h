
#include "C:\Users\Mikes 6700K\.platformio\packages\framework-arduinoespressif8266\cores\esp8266\Arduino.h"
#include "C:\Users\Mikes 6700K\Dropbox\sketchbook\RMS slave\lib\Time-master/TimeLib.h"
#include <SPISlave.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <WiFiUdp.h>
#include <fs.h>
#include <ESP8266mDNS.h>
#include <Ticker.h>
#include <ArduinoOTA.h>
extern "C" {
#include "user_interface.h"
}

uint8_t init_OTA();
uint8_t listDiags();
uint32_t getTime();
void setupSPIslave();
char * timeStamp();
char * dateStamp();
char* f2s2(float f);
char* f2s4(float f);
char* i2sd(uint8_t b);
char* i2sh(uint8_t b);
void diagMess(const char*);
void errMess(const char*);
void helpPage();
void handleMetrics();
void handleNotFound();
void ISRwatchDog();
void waitForData();
void qtrProc();
void watchWait(uint32_t);
void addCstring(const char* s);
void dirList();

#define NTP_PACKET_SIZE 48
#define TIME_ZONE 10
#define HTML_SIZE 10000
#define NUM_CHANNELS 7

extern WiFiUDP udp;
extern WiFiClient client;
extern FSInfo fs_info;
extern File fh,fd,fe;
extern Ticker secondTick;
extern volatile uint8_t watchDog;
extern IPAddress localIP,timeServerIP,fileServerIP;

extern bool noData;
extern ESP8266WebServer server;

extern char fileName[];
extern char todayName[];
extern char userText[];
extern char saveName[];
extern char dateStr[];
extern char timeStr[];
extern char charBuf[];
extern char d2Str[];
extern char fltStr[];
extern char htmlStr[];            

extern uint8_t buffer[];
extern uint8_t SPIdata[];
extern uint8_t oldMin,oldQtr,oldHour,oldDay,oldMonth;
extern uint16_t i,oldYear,htmlLen;
extern uint16_t localPort;   
extern uint32_t t0, t1, startMillis, startSeconds, midNight;

extern float Wrms[];					    // Sum of sampled V*I
extern float Irms[];					    // root sum I^2
extern float Vrms,Vmin,Vmax;			// root sum V^2, -Vp, +Vp
extern float Freq;
