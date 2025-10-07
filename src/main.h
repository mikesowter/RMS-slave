#include "defines.h"
#include "Arduino.h"
#include "TimeLib.h"
#include "secrets.h"
#include <SPISlave.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <SimpleFTPServer.h>
#include <WiFiudp.h>
#include <LittleFS.h>
#include <ESP8266mDNS.h>
#include <Ticker.h>
#include <ArduinoOTA.h>
extern "C" {
#include "user_interface.h"
}

uint8_t init_OTA();
unsigned long getTime();
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
void watchWait(unsigned long);
void joinNet();
void setupTime();
void updateEnergyFile();
void checkScan();
void readPromDB();
// void fillBarrel();
void setupFS();
void sync2Master();
void calcCheckSum();
void writeDemands();

String resetReason = "Restart caused by " + ESP.getResetReason();
String resetDetail = ESP.getResetInfo();

ESP8266WebServer server( 80 );
FtpServer ftpSrv;
WiFiUDP udp;
WiFiClient client;
FSInfo fs_info;
File fh,fd,fe;
Ticker secondTick;
volatile uint8_t watchDog;

bool SPIwait = true;     // wait for data in SPI slave loop?
bool noDataYet = true;   // no data from RMS master yet?
bool dataTimeout;        // 30s timeout exceeded.
bool checkSumBad;
bool waterOn, exporting, exporting7_5, exporting10, scanFail;
bool T31charging, pwrOutage, peakPeriod;
bool FTPcheck;
bool barrelFail;

bool newArg;             // new parameter for database
uint8_t gobackhrs = 0;     // get scan data from database?
char fileName[20];
char todayName[] = "/XXXyymmdd.csv";
char userText[30];
char saveName[20];
char dateStr[] = "yymmdd ";
char timeStr[] = "hh:mm:ss.ss,";    // in diagnostic mode only
char charBuf[256];
char d2Str[] = "01";
char d8Str[8];
char fltStr[12];
char longStr[LONG_STR_SIZE];  
float avgWatts(float w, uint8_t cct, uint8_t d);
IPAddress localIP, timeServerIP;
IPAddress ip(192, 168, 1, IP4); 
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress dns(192, 168, 1, 1);

uint8_t MOSIdata[32], MISOdata[32];
uint8_t wattsIndex;
uint8_t buffer[BUFFER_SIZE];
uint8_t oldMin, old5Min, oldQtr, oldHour, oldDay, oldMonth, offset;
uint8_t scanSec, badSumCount = 0;
uint16_t oldYear, i, htmlLen, CstringPtr, syncDelay = 800;
uint16_t localPort = 4210;          //  must match port assigned in "pulse"
uint16_t loopCount = 0, missedCycle = 0;
unsigned long t0, t1, t2, t3, startMillis, startSeconds, lastScan, startOutage;
unsigned long t_lastData, t_scan, t_scan_max, t_scan_min = 9999UL, t_scanSum, waiting, WWmin, WWmax, WDmin, WDmax;
unsigned long loopStart, loopTime, LTmin, LTmax, progLoopTime;
unsigned long wfdStart, wfdTime, wfdPrev, WFDmin, WFDmax;

float Wrms[MAX_CCTS+1];					// Sum of sampled V*I
/*  RMS1:
    scan group    load
    cct0 Wrms[1]  total house load
    cct1 Wrms[2]  bedrooms 1&2
    cct2 Wrms[3]  kitchen lounge
    cct3 Wrms[4]  downstairs
    cct4 Wrms[5]  hotwater
    cct5 Wrms[6]  oven
    cct6 Wrms[7]  solar contribution
    cct7 Wrms[8]  lights    

    RMS2:
    cct3 Wrms[3]  cct7 phase
    cct4 Wrms[4]  garage aircon
    cct5 Wrms[5]  garage power
    cct6 Wrms[6]  garage lights
    cct7 Wrms[7]  export power
*/
float Energy[NUM_CCTS+1];	
float incEnergy[NUM_CCTS+1];
float costEnergy[3][NUM_CCTS+1];   
/*  RMS1
    costEnergy[ps][1] is cost of unmetered (tier2loads) in $
    costEnergy[ps][2-6] is cost of Wrms[2-6]
    costEnergy[ps][7] is income from FIT_rate
    costEnergy[ps][8] is cost of lights
*/
float Wrms_min[NUM_CCTS+1];		        // in Watts
float Wrms_max[NUM_CCTS+1];	
float Wrms_avg[NUM_CCTS+1];	
float Wimp, Wexp;
float wattsBarrel[8][8];
float Arms[NUM_CCTS+1];				    // root sum I^2
float Vrms, Vpk_min, Vpk_max;		    // root sum V^2, -Vp, +Vp
float Freq = 50.0F;                     // grid frequency to 50.01 res
float Vrms_min = 500.0F;                // max values between scans
float Vrms_max = 0.0F;
float Vmin_n = 500.0F, Vmin_p = 500.0F;
float Vmax_n = 0.0F, Vmax_p = 0.0F;
float loads, solar;
float avSparekW = 0.0F;                 // smoothed solar surplus
float Vbat;
float rms5Demand, rms15Demand, rms30Demand, rms15Peak, rms30Peak;
float FI_5m_kWh[6], FI_5m_kW, FI_15m_kW, FI_30m_kW;
double Imp_meter,Exp_meter,Imp_5m_kWh[6];
float T11_kWh[3];           // daily sum from grid with each panel size
float T11_inc[3];           // kwH increment from grid
float FIT_kWh[3];           // daily sum to grid with each panel size
float FIT_inc[3];           // kwH increment to grid
float T11_W;
float I7phase;



