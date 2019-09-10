#include <arduino.h>
#include <WiFiClient.h>
#include <ESP8266WiFi.h>
#include <TimeLib.h>
#include <WiFiUdp.h>

void sendNTPrequest(IPAddress& address);
unsigned long getNTPreply();
void diagMess(const char* mess);
char* dateStamp();
char* timeStamp();

uint32_t Trefsec, Trxsec, Ttxsec, Trefusec, Trxusec, Ttxusec, ausSeconds; 

extern WiFiUDP udp;

const uint8_t NTP_PACKET_SIZE = 48;
const uint8_t BUFFER_SIZE = 100;
uint8_t len;
uint8_t byteBuf[BUFFER_SIZE];
IPAddress ausTimeServerIP,localTimeServerIP={192,168,1,6};
const char* ntpServerName = "au.pool.ntp.org";
const uint8_t TIME_ZONE = 10;
const uint32_t TO_1970 = 2208988800UL;
unsigned long from_1900, GMT;   

/* convert microseconds to fraction of second * 2^32 (i.e., the lsw of
   a 64-bit ntp timestamp).  This routine uses the factorization:
    n microseconds = n*(4096 + 256 - 1825/32)/2^32 seconds
   which results in a max conversion error of 0.34us */

unsigned int usec2ntp(unsigned int usec) {
  unsigned int t = (usec * 1825) >> 5;
  return ((usec << 12) + (usec << 8) - t);
}

unsigned long getTime() {
  //
  //   Aus pool server
  //
  WiFi.hostByName(ntpServerName, ausTimeServerIP);
  uint32_t ms = millis();
  while ( millis() - ms < 5000 ) {
    sendNTPrequest(ausTimeServerIP);
    delay(2000);
    uint32_t len = udp.parsePacket();
    Serial.printf("Aus NTP server ------------------ \n\npacket size: %i\n",len);
    if (len > 40 ) break;
  }
  ausSeconds = getNTPreply();
  setTime(Trefsec);
  char usecs[] = ".00000000";
  double two32 = (float)0XFFFFFFFF/1.0e6;
  dtostrf((double)Trefusec/two32, 6, 0, usecs);
  Serial.printf("\n reference time: %s %s %sµs\n", dateStamp(), timeStamp(),usecs);
  setTime(Trxsec);
  dtostrf((double)Trxusec/two32, 6, 0, usecs);
  Serial.printf("\n received time:  %s %s %sµs\n", dateStamp(), timeStamp(),usecs);
  setTime(Ttxsec);
  dtostrf((double)Ttxusec/two32, 6, 0, usecs);
  Serial.printf("\n transmit time:  %s %s %sµs\n", dateStamp(), timeStamp(),usecs); 
//  
//    local NTP
/*
  if ( true ) {
    setTime(ausSeconds);
    Serial.printf("time: %s %s\n\n", dateStamp(),timeStamp());
  } 

  ms = millis();
  while ( millis() - ms < 300 ) {
    sendNTPrequest(localTimeServerIP);
    delay(1000);
    uint32_t len = udp.parsePacket();
    Serial.printf("local NTP server --------------- \n\npacket size: %i\n",len);
    if (len > 40 ) break;
  }
  startSeconds = getNTPreply();
  if ( true ) {
    setTime(startSeconds);
    Serial.printf("time: %s %s\n", dateStamp(),timeStamp());
    return ausSeconds;
  } */
  return Ttxsec;
}

// send an NTP request to the time server at the given address
void sendNTPrequest(IPAddress& address)
{
  memset(byteBuf, 0, BUFFER_SIZE);
  // Initialize values needed to form NTP request
  byteBuf[0] = 0b11100011;   // LI, Version, Mode
  byteBuf[1] = 0;     // Stratum, or type of clock
  byteBuf[2] = 6;     // Polling Interval (2^6s)
  byteBuf[3] = 0xEC;  // Peer Clock Precision (2^-19s)
  // 8 bytes of zero for Root Delay & Root Dispersion, then ID:
  byteBuf[12]  = 'M';
  byteBuf[13]  = 'J';
  byteBuf[14]  = 'S';
  byteBuf[15]  = '!';
  // 32 bytes of zero because our time doesnt matter here

  udp.beginPacket(address, 123); 
  udp.write(byteBuf, NTP_PACKET_SIZE);
  udp.endPacket();
}

unsigned long getNTPreply() {
  udp.read(byteBuf, NTP_PACKET_SIZE); 
  Serial.println("\nNTP reply: ");
  for (int p=0; p<NTP_PACKET_SIZE; p=p+4) {
    Serial.printf("word %0i: %0X,%0X,%0X,%0X\n",p/4, byteBuf[p], byteBuf[p+1], byteBuf[p+2], byteBuf[p+3]);
  } 
// get Reference time:
  from_1900 = (byteBuf[16]<<24) + (byteBuf[17]<<16) + (byteBuf[18]<<8) + byteBuf[19];   
  GMT = from_1900 - TO_1970;        
  Trefsec = GMT + TIME_ZONE*3600;
  Trefusec = usec2ntp( (byteBuf[20]<<24) + (byteBuf[21]<<16) + (byteBuf[22]<<8) + byteBuf[23] );

// get Receive time:
  from_1900 = (byteBuf[32]<<24) + (byteBuf[33]<<16) + (byteBuf[34]<<8) + byteBuf[35]; 
  GMT = from_1900 - TO_1970;        
  Trxsec = GMT + TIME_ZONE*3600;
  Trxusec = usec2ntp( (byteBuf[36]<<24) + (byteBuf[37]<<16) + (byteBuf[38]<<8) + byteBuf[39] );

// get Transmit time:   
  from_1900 = (byteBuf[40]<<24) + (byteBuf[41]<<16) + (byteBuf[42]<<8) + byteBuf[43]; 
  GMT = from_1900 - TO_1970;        
  Ttxsec = GMT + TIME_ZONE*3600;
  Ttxusec = usec2ntp( (byteBuf[44]<<24) + (byteBuf[45]<<16) + (byteBuf[46]<<8) + byteBuf[47] ); 

  return Ttxsec;
}
