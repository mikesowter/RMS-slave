
#include "Arduino.h"
#include "TimeLib.h"
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
void handleWater();
void handleNotFound();
void ISRwatchDog();
void waitForData();
void minProc();
void watchWait(uint32_t);

String resetReason = "\nreset: " + ESP.getResetReason();
#define ntpServerName "au.pool.ntp.org"
#define NTP_PACKET_SIZE 48
#define BUFFER_SIZE 128
#define TIME_ZONE 10
#define longStrSize 10000
#define NUM_CHANNELS 8

ESP8266WebServer server( 80 );
WiFiUDP udp;
WiFiClient client;
FSInfo fs_info;
File fh,fd,fe;
Ticker secondTick;
volatile uint8_t watchDog = 0;

bool noData = true, waterOn;

char fileName[] = "/XXyymmdd.csv";
char todayName[] = "/XXyymmdd.csv";
char userText[20];
char saveName[20];
char dateStr[] = "yymmdd";
char timeStr[] = "hh:mm:ss";
char charBuf[256];
char d2Str[] = "01";
char fltStr[12];
char longStr[longStrSize];            // use C strings for storage efficiency

IPAddress localIP,timeServerIP,fileServerIP;
IPAddress ip(192, 168, 1, 56);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress dns(192, 168, 1, 1);

uint8_t SPIdata[64];
uint8_t buffer[BUFFER_SIZE];
uint8_t oldMin,oldQtr,oldHour,oldDay,oldMonth,offset;
uint16_t i,oldYear,htmlLen;
uint16_t localPort = 2395;          //  a random local port for UDP packets
uint32_t t0, t1, startMillis, startSeconds, midNight;

float Wrms[NUM_CHANNELS+1];					// Sum of sampled V*I
float Wrms_min[NUM_CHANNELS+1];		
float Wrms_max[NUM_CHANNELS+1];	
float Irms[NUM_CHANNELS+1];					// root sum I^2
float Vrms=245.0,Vmin=500.0,Vmax;		// root sum V^2, -Vp, +Vp
float Freq;                         // grid frequency to 50.000
float Vrms_min = 500.0;   // max values between scans
float Vrms_max = 0.0;
float Vmin_15 = 500.0;
float Vmax_15 = 0.0;
  
