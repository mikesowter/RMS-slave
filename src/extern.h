#include "defines.h"
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
void joinNet();
byte storeData();
void dailyEnergy();
void batteryEnergy();
void readPromDB();
void updateEnergyFile();
void updateBatteryFile();
void setupTime();
void calcCheckSum();
void activity(const char* tag);
void writeImportExport();

extern uint8_t gobackhrs;
extern WiFiUDP udp;
extern WiFiClient client;
extern FtpServer ftpSrv;
extern FSInfo fs_info;
extern File fh,fd,fe;
extern Ticker secondTick;
extern volatile uint8_t watchDog;
extern IPAddress localIP,timeServerIP,fileServerIP;
extern bool SPIwait, waterOn, peakPeriod;
extern bool checkSumBad;
extern bool exporting, exporting7_5, exporting10, T31charging, pwrOutage;
extern bool noDataYet, dataTimeout;
extern bool FTPcheck;
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
extern uint8_t MOSIdata[], MISOdata[];
extern uint8_t oldMin, old5Min, oldQtr, oldHour, oldDay, oldMonth, offset;
extern uint16_t i, oldYear, htmlLen, CstringPtr;
extern uint16_t localPort, loopCount, missedCycle;   
extern uint32_t t0, t1, t2, t3, startMillis, startSeconds, lastScan;
extern uint32_t t_lastData, t_scan, t_scan_max, waiting, WWmin, WWmax, WDmin, WDmax;
extern uint32_t loopStart, loopTime, LTmin, LTmax;
extern uint32_t wfdStart, wfdTime, wfdPrev, WFDmin, WFDmax;
extern uint32_t waitStart, appStart;

extern float Wrms[];					    // Sum of sampled V*I
extern float Arms[];					    // root sum I^2
extern float Vrms;			                // root sum V^2
extern float Freq;
extern float Vrms_min;
extern float Vrms_max;
extern float Vmin_n, Vmin_p;
extern float Vmax_n, Vmax_p;
extern float Wrms_min[];		
extern float Wrms_max[];	
extern float Energy[],E3,E7;	
extern float incEnergy[], costEnergy[][9];
extern float T11_W, T11_kWh[], T11_inc[], FIT_kWh[], FIT_inc[];
extern float T11_5m_kWh[], rms5Demand, rms15Demand, rms30Demand, rms15Peak, rms30Peak;
extern float FI_5m_kWh[], FI_5m_kW, FI_15m_kW, FI_30m_kW;
extern double T11_meter,FI_meter;
extern float panelCap[];
extern float BattCap[];
extern float excessSolar[], batt_savings[3][3];    // first index is solar, 2nd is battery size
extern float batt_tohouse[3][3], batt_charge[3][3], dump_togrid[3][3];
extern float loads, solar, avSparekW;
extern float Vbat;

