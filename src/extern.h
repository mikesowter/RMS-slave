
#include "Arduino.h"
#include "TimeLib.h"
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
uint8_t listDiags();
uint32_t getTime();
void setupSPIslave();
char* timeStamp();
char* dateStamp();
char* f2s2(float f);
char* f2s4(float f);
char* i2sd(uint8_t b);
char* i2sh(uint8_t b);
void diagMess(const char*);
void errMess(const char*);
void helpPage();
void handleMetrics();
void handleWater();
void handleNotFound();
void ISRwatchDog();
void waitForData();
void minProc();
void watchWait(uint32_t);
void addCstring(const char* s);
void dirList();
void joinNet();
byte storeData();
void dailyEnergy();
void batteryEnergy();
void t31check();
void updateEnergyFile();
void updateBatteryFile();
void setupTime();

#define NTP_PACKET_SIZE 48
#define TIME_ZONE 10
#define longStrSize 10000
#define NUM_CIRCUITS 8 
#define FS_ID LittleFS

extern WiFiUDP udp;
extern WiFiClient client;
extern FtpServer ftpSrv;
extern FSInfo fs_info;
extern File fh,fd,fe;
extern Ticker secondTick;
extern volatile uint8_t watchDog;
extern IPAddress localIP,timeServerIP,fileServerIP;
extern bool noData, waterOn, exporting, T31charging, pwrOutage;
extern ESP8266WebServer server;

extern char fileName[];
extern char todayName[];
extern char userText[];
extern char saveName[];
extern char dateStr[];
extern char timeStr[];
extern char charBuf[];
extern char d2Str[];
extern char d8Str[];
extern char fltStr[];
extern char longStr[];            

extern uint8_t buffer[];
extern uint8_t scanSec;
extern uint8_t SPIdata[];
extern uint8_t oldMin, oldQtr, oldHour, oldDay, oldMonth, offset;
extern uint16_t i, oldYear, htmlLen, CstringPtr;
extern uint16_t localPort;   
extern uint32_t t0, t1, startMillis, startSeconds, lastScan;
extern uint32_t t_lastData, t_scan, waiting, WWmin, WWmax, WDmin, WDmax;

extern float T31;
extern float T11;
extern float FIT;
extern float Wrms[];					    // Sum of sampled V*I
extern float Irms[];					    // root sum I^2
extern float Vrms,Vpk_neg,Vpk_pos;			// root sum V^2, -Vp, +Vp
extern float Freq;
extern float Vrms_min;
extern float Vrms_max;
extern float Vmin_n, Vmin_p;
extern float Vmax_n, Vmax_p;
extern float Wrms_min[];		
extern float Wrms_max[];	
extern float Energy[];	
extern float incEnergy[];
extern float costEnergy[];
extern float batt_charge;
extern float batt_tohouse, batt_togrid;
extern float batt_savings, batt_costs;
extern float batt_charge75;
extern float batt_tohouse75, batt_togrid75;
extern float batt_savings75, batt_costs75;
extern float loads, solar, avSparekW;
extern float T11_kWh, T11_inc;
extern float Vbat;
