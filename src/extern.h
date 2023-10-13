
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
char* f2s5(float f);
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
void dirList();
void joinNet();
byte storeData();
void dailyEnergy();
void batteryEnergy();
void readPromDB();
// void t31check();
void updateEnergyFile();
void updateBatteryFile();
void setupTime();
bool calcChecksum();

#define NTP_PACKET_SIZE 48
#define TIME_ZONE 10
#define LONG_STR_SIZE 10000
#define FS_ID LittleFS
#define MASTER_RESET D3
#define LED_PIN D4

//#define RMS2
#ifdef RMS2
    #define NUM_CCTS 7
#else
    #define NUM_CCTS 8
    extern float T11_kWh[];
#endif



extern uint8_t gobackhrs;
extern WiFiUDP udp;
extern WiFiClient client;
extern FtpServer ftpSrv;
extern FSInfo fs_info;
extern File fh,fd,fe;
extern Ticker secondTick;
extern volatile uint8_t watchDog;
extern IPAddress localIP,timeServerIP,fileServerIP;
extern bool SPIwait, waterOn;
extern bool checkSumBad;
extern bool exporting, exporting7_5, exporting10, T31charging, pwrOutage;
extern bool noDataYet;
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
extern uint8_t scanSec, badSumCount;
extern uint8_t SPIdata[];
extern uint8_t oldMin, oldQtr, oldHour, oldDay, oldMonth, offset;
extern uint16_t i, oldYear, htmlLen, CstringPtr;
extern uint16_t localPort, missedCycle;   
extern uint32_t t0, t1, startMillis, startSeconds, lastScan;
extern uint32_t t_lastData, t_scan, waiting, WWmin, WWmax, WDmin, WDmax;
extern uint32_t loopStart, loopTime, LTmin, LTmax;
extern uint32_t wfdStart, wfdTime, wfdPrev, WFDmin, WFDmax;

extern float Wrms[];					    // Sum of sampled V*I
extern float Arms[];					    // root sum I^2
extern float Vrms,Vpp,Vnp;			// root sum V^2, -Vp, +Vp
extern float Freq;
extern float Vrms_min;
extern float Vrms_max;
extern float Vmin_n, Vmin_p;
extern float Vmax_n, Vmax_p;
extern float Wrms_min[];		
extern float Wrms_max[];	
extern float Energy[];	
extern float incEnergy[], costEnergy[];

extern float panelCap[];
extern float BattCap[];
extern float excessSolar[], batt_savings[3][3];    // first index is solar, 2nd is battery size
extern float batt_tohouse[3][3], batt_charge[3][3], dump_togrid[3][3];

extern float loads, solar, avSparekW;
extern float Vbat;
