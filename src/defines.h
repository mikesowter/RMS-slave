// a single location for defining variations between implementations of the RMS hardware

#define RMS1

#ifdef RMS1
    #define NUM_CCTS 8
    #define MAX_CCTS 11
    #define CODEVERSION "\nRMS1 slave 20250409"
    #define IP4 56
    #define FIRST_CCT 1
#endif
#ifdef RMS2
    #define NUM_CCTS 7
    #define MAX_CCTS 7
    #define CODEVERSION "\nRMS2 slave 20241113"
    #define IP4 62
    #define FIRST_CCT 4
#endif

#define FS_ID LittleFS
#define NTP_SERVER_NAME "au.pool.ntp.org"
#define NTP_PACKET_SIZE 48
#define BUFFER_SIZE 128
#define TIME_ZONE 10
#define LONG_STR_SIZE 10000
#define MASTER_RESET D3
#define LED_PIN D4
#define T31 0.18172             // updated 20240701
#define Tariff11 0.2717         // updated 20240701
#define FeedInTariff 0.04       // updated 20250301
#define DemandTariff 0.18546    // updated 20240701

