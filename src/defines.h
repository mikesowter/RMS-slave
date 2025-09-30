// a single location for defining variations between implementations of the RMS hardware

#define RMS1

#ifdef RMS1
    #define NUM_CCTS 8
    #define MAX_CCTS 11
    #define CODEVERSION "\nRMS1 slave 20250909"
    #define IP4 56
    #define FIRST_CCT 1
#endif
#ifdef RMS2
    #define NUM_CCTS 7
    #define MAX_CCTS 7
    #define CODEVERSION "\nRMS2 slave 20250416"
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
#define T31 0.17754             // updated 20250421
#define T11_high 0.4107         // updated 20250909
#define T11_med 0.3093          // updated 20250909
#define T11_low 0.2640          // updated 20250909
#define FIT_high 0.241          // updated 20250929
#define FIT_med 0.103           // updated 20250929
#define FIT_low 0.033           // updated 20250929
#define DemandTariff 0.0        // to be removed

