
#include "Arduino.h"
#include "TimeLib.h"
#include "secrets.h"
#include <SPISlave.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESPFTPServer.h>
#include <WiFiudp.h>
#include <LittleFS.h>
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
void handleRoot();
void handleDir();
void handleMetrics();
void handleWater();
void handleNotFound();
void ISRwatchDog();
void waitForData();
void minProc();
void watchWait(uint32_t);
void joinNet();
void setupTime();
void updateEnergyFile();
void checkScan();
void getLastScan();
void setupFS();
void sync2Master();

String resetReason = "Restart caused by " + ESP.getResetReason();
String resetDetail = ESP.getResetInfo();

#define ntpServerName "au.pool.ntp.org"
#define NTP_PACKET_SIZE 48
#define BUFFER_SIZE 128
#define TIME_ZONE 10
#define longStrSize 10000
#define NUM_CIRCUITS 8
#define LED_PIN D4

ESP8266WebServer server( 80 );
FtpServer ftpSrv;
WiFiUDP udp;
WiFiClient client;
FSInfo fs_info;
File fh,fd,fe;
Ticker secondTick;
volatile uint8_t watchDog = 0;

bool SPIwait = true;     // wait for data in SPI slave loop
bool noDataYet = true;  // no data from RMS master yet, values false
bool waterOn, exporting, exporting7_5, exporting10, scanFail, T31charging, pwrOutage;
char fileName[] = "/XXXyymmdd.csv";
char todayName[] = "/XXXyymmdd.csv";
char userText[30];
char saveName[20];
char dateStr[] = "yymmdd";
char timeStr[] = "hh:mm:ss ";
char charBuf[256];
char d2Str[] = "01";
char d8Str[8];
char fltStr[12];
char longStr[longStrSize];  

IPAddress localIP, timeServerIP;
IPAddress ip(192, 168, 1, 56); 
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress dns(192, 168, 1, 1);

uint8_t SPIdata[64];
uint8_t buffer[BUFFER_SIZE];
uint8_t oldMin, oldQtr, oldHour, oldDay, oldMonth, offset;
uint8_t scanSec;
uint16_t i, oldYear, htmlLen, CstringPtr, syncDelay = 800;
uint16_t localPort = 4210;          //  must match port assigned in "pulse"
uint32_t t0, t1, startMillis, startSeconds, lastScan;
uint32_t t_lastData, t_scan, waiting, WWmin, WWmax, WDmin, WDmax;
uint32_t loopStart, loopTime, LTmin, LTmax;
uint32_t wfdStart, wfdTime, WFDmin, WFDmax;


float T31 = 0.166;    // updated 20220901
float T11 = 0.241;    // updated 20220901
float FIT = 0.08;     // updated 20211128

float Wrms[NUM_CIRCUITS+1];					// Sum of sampled V*I
/*  scan Wrms[] load
    cct0 1  total import to house
    cct1 2  bedrooms 1&2
    cct2 3  kitchen lounge
    cct3 4  downstairs
    cct4 5  hotwater
    cct5 6  oven
    cct6 7  solar
    cct7 8  lights    */
float Energy[NUM_CIRCUITS+1];	
float incEnergy[NUM_CIRCUITS+1];
float batt_charge = 1.0, batt_togrid = 0.0;
float costEnergy[NUM_CIRCUITS+1];   // costEnergy[1] is cost of unmetered
float Wrms_min[NUM_CIRCUITS+1];		
float Wrms_max[NUM_CIRCUITS+1];	
float Irms[NUM_CIRCUITS+1];					// root sum I^2
float Vrms, Vpk_min, Vpk_max;		    // root sum V^2, -Vp, +Vp
float Freq;                         // grid frequency to 50.000
float Vrms_min = 500.0;             // max values between scans
float Vrms_max = 0.0;
float Vmin_n = 500.0, Vmin_p = 500.0;
float Vmax_n = 0.0, Vmax_p = 0.0;
float T11_kWh = 0.0;        // daily sum from grid
float T11_inc;              // increment from grid
float batt_tohouse = 0.0;   // daily sum from battery
float loads, solar;
float batteryFlow, batt_savings, batt_costs;
float avSparekW;            // smoothed solar-loads
float Vbat;
// 7.5kW simulation
float batteryFlow7_5, batt_savings7_5, batt_costs7_5;
float T11_kWh7_5 = 0.0;       // daily sum from grid with 7.5kW panels
float batt_tohouse7_5 = 0.0;  // daily sum from battery
float batt_charge7_5 = 1.0; 
float batt_togrid7_5 = 0.0;
float T11_inc7_5;             // increment from grid with 7.5kW panels
// 10kW simulation
float batteryFlow10, batt_savings10, batt_costs10;
float T11_kWh10 = 0.0;       
float batt_tohouse10 = 0.0;  
float batt_charge10 = 1.0;
float batt_togrid10 = 0.0;
float T11_inc10;             


