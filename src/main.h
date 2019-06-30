
#include "C:\Users\Mikes 6700K\.platformio\packages\framework-arduinoespressif8266\cores\esp8266\Arduino.h"
#include "C:\Users\Mikes 6700K\Dropbox\sketchbook\RMS slave\lib\Time-master\TimeLib.h"
#include "secrets.h"
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
uint32_t getTime();
void setupSPIslave();
char* timeStamp();
char* dateStamp();
char* i2sh(uint8_t b);
void diagMess(const char*);
void errMess(const char*);
void handleMetrics();
void handleNotFound();
void ISRwatchDog();
void waitForData();
void qtrProc();
void watchWait(uint32_t);

String resetReason = ESP.getResetReason();
#define NTP_PACKET_SIZE 48
#define BUFFER_SIZE 128
#define TIME_ZONE 10
#define HTML_SIZE 10000
#define NUM_CHANNELS 7
#define ntpServerName "au.pool.ntp.org"

ESP8266WebServer server( 80 );
WiFiUDP udp;
WiFiClient client;
FSInfo fs_info;
File fh,fd,fe;
Ticker secondTick;
volatile uint8_t watchDog = 0;

bool noData = true;

char fileName[] = "/XXyymmdd.csv";
char todayName[] = "/XXyymmdd.csv";
char userText[20];
char saveName[20];
char dateStr[] = "yymmdd";
char timeStr[] = "hh:mm:ss";
char charBuf[128];
char d2Str[] = "01";
char fltStr[12];
char htmlStr[HTML_SIZE];            // use C strings for storage efficiency

IPAddress localIP,timeServerIP,fileServerIP;
IPAddress ip(192, 168, 1, 56);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress dns(192, 168, 1, 1);

uint8_t buffer[BUFFER_SIZE];
uint8_t SPIdata[32];
uint8_t oldMin,oldQtr,oldHour,oldDay,oldMonth;
uint16_t i,oldYear,htmlLen;
uint16_t localPort = 2391;          //  a random local port for UDP packets
uint32_t t0, t1, startMillis, startSeconds, midNight;

float Wrms[NUM_CHANNELS];					  // Sum of sampled V*I
float Irms[NUM_CHANNELS];					  // root sum I^2
float Vrms=245.0,Vmin,Vmax;					// root sum V^2, -Vp, +Vp
float Freq;                         // grid frequency to 50.000
