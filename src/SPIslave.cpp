#include "extern.h"

float unload2Bytes();
bool unloadValues();
const float AMPS_CORRECTION_FACTOR = 1.021; // low currents < 4A on 20190922
uint16_t checkSum;

void setupSPIslave() {
  // data has been received from the master. len is always 32 bytes
  SPISlave.onData([](uint8_t * data, size_t len) {
    SPISlave.setData(data,32);
    for (uint8_t i=0; i<32; i++) SPIdata[i] = data[i];
    SPIwait = false;
  });

  // The master has read out outgoing data buffer
  // that buffer can be set with SPISlave.setData
  SPISlave.onDataSent([]() {  });

  // status has been received from the master.
  // The status register is a special register that both the slave and the master can write to and read from.
  // Can be used to exchange small data or status information
  SPISlave.onStatus([](uint32_t data) {
    Serial.printf("Master status: %u\n", data);
  });

  // The master has read the status register
  SPISlave.onStatusSent([]() {
    Serial.println("Time code read\n");
  });
  // Setup SPI Slave registers and pins
  SPISlave.begin();

  // Set the status register (if the master reads it, it will read this value)
  SPISlave.setStatus(now());
  Serial.println("SPI slave started" );
}

void waitForData() {
  digitalWrite(LED_PIN,0);
  wfdPrev = wfdTime;
  wfdStart = millis();
  do {
    SPISlave.begin();
    while (SPIwait) watchWait(10);
    SPIwait = true;
    SPISlave.end(); 
  } while ( !checkSumOk() ); // bad checksum

  // measure WFD times
  wfdTime = millis()-wfdStart;
  WFDmin = _min(WFDmin,wfdTime);
  WFDmax = _max(WFDmax,wfdTime);
  unloadValues();
  dailyEnergy();
  yield(); 
  batteryEnergy();      
  noDataYet = false;   
  digitalWrite(LED_PIN,1);
}

bool unloadValues() {
  if ( SPIdata[0] == 0xFF && SPIdata[1] == 0xFF) {
    if ( !pwrOutage ) {
      pwrOutage = true;
      diagMess("power outage");
    }
    Freq = 0.0;
    Vrms = 0.0;           
    Vrms_min = 0.0;      
    Vrms_max = 0.0;      
    Vmin_n = 0.0;      
    Vmax_n = 0.0;      
    Vmin_p = 0.0;      
    Vmax_p = 0.0; 
    for (int i=1;i<9;i++) {
      Wrms_min[i] = 0.0F;
      Wrms_max[i] = 0.0F;
    }
    noDataYet = false;   
  }
  else {
    if ( pwrOutage ) {
      pwrOutage = false;
      diagMess("power restored");
    }
    if ( !checkSumOk() ) return false;
    offset = 0;
    float v,w;
    Freq = unload2Bytes()/1000.0;
    if (Freq < 40.0 || Freq > 55.0 ) Freq = 50.0;   // remove freq errors from record
    Vrms = unload2Bytes()/100.0;                
    Vrms_min = _min(Vrms_min,Vrms);
    Vrms_max = _max(Vrms_max,Vrms);
    v = unload2Bytes()/50.0;    // peak from negative half cycle
    Vmin_n = _min(Vmin_n,v);
    Vmax_n = _max(Vmax_n,v);
    v = unload2Bytes()/50.0;    // peak from positive half cycle
    Vmin_p = _min(Vmin_p,v);
    Vmax_p = _max(Vmax_p,v);
    // Serial.printf("Freq = %0.3f Vrms=%0.1f Vmin=%0.1f Vmax=%0.1f\n", Freq, Vrms, Vmin, Vmax);

    if ( Vmin_n > -400.0 && Vmax_p < 400.0) {       // remove spurious surge power from the record
      for (uint8_t p=1 ; p<(NUM_CIRCUITS+1) ; p++) { 
        w = unload2Bytes();
        if ( w > 15000 ) w = 9999;                  // reasonability limit
        else if ( w < 5 ) w = 0.0;                  // remove low end noise
        Wrms[p] = (float)w;    
        Wrms_min[p] = _min( Wrms_min[p], w );
        Wrms_max[p] = _max( Wrms_max[p], w ); 
      }
    }
    waterOn = false;
    if ( Wrms[5] > 1000 ) waterOn = true;
    // for big load queries (time to start/stop the pool heater?)
    avSparekW = 0.99*avSparekW + 0.01*( Wrms[7] - Wrms[1] );  

  }
  offset = 28;
  Vbat = unload2Bytes()/2070.0F;
  return true;
}

float unload2Bytes() {
  if (offset > 30) {
    diagMess("illegal SPI data offset ");
    return 0.0;
  }
  uint16_t val = 256.0*SPIdata[offset++];
  val += SPIdata[offset++];
  checkSum += val;
  return (float)val;
}

bool checkSumOk() {
  checkSum = 0;
  offset = 0;
  while (offset<29) unload2Bytes();  
  
  uint16_t rxSum = checkSum;                   // sum of bytes 0-29 calc'd by receiver
  uint16_t txSum = (uint16_t) unload2Bytes();  // sum of bytes 0-29 calc'd by transmitter
  if (rxSum == txSum) return true;

  sprintf(charBuf,"checksum");
  diagMess(charBuf);
  if ( ++badSumCount < 3 ) return false;
  badSumCount = 0;
  sprintf(charBuf,"rebooting master");
  diagMess(charBuf);
  digitalWrite(MASTER_RESET,0);
  delayMicroseconds(100);
  digitalWrite(MASTER_RESET,1);
  delay(1000);
//  sprintf(charBuf,"rebooting slave");
//  diagMess(charBuf);

//  ESP.restart();
  return true;   // needed for compiler
}