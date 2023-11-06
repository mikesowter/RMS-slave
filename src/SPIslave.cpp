#include "extern.h"

float unload2Bytes();
bool unloadValues();
const float AMPS_CORRECTION_FACTOR = 1.021; // low currents < 4A on 20190922
uint16_t checkSum;
float v,w;

void setupSPIslave() {
  // data has been received from the master. len is always 32 bytes
  SPISlave.onData([](uint8_t * data, size_t len) {
    SPISlave.setData(data,32);
    for (uint8_t i=0; i<32; i++) SPIdata[i] = data[i];
    SPIwait = false;
  });

  // The master has read outgoing data buffer
  // that buffer can be set with SPISlave.setData
  SPISlave.onDataSent([]() {  });

  // status has been received from the master.
  // The status register is a special register that both the slave and the master can write to and read from.
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
    if (millis() > wfdStart + 30000) break;  // for minProc
    calcCheckSum();
    Serial.print("\n");  
    Serial.print(timeStamp());
    for ( uint8_t p=0;p<32;p+=2 ) {
#ifdef RMS1
      if ( p > 3 && p < 22) Serial.printf("%d,",(int16_t)(256*SPIdata[p]+SPIdata[p+1]));
      else Serial.printf("%d,",256*SPIdata[p]+SPIdata[p+1]);
#endif
#ifdef RMS2
      if ( p > 7 && p < 22) Serial.printf("%d,",(int16_t)(256*SPIdata[p]+SPIdata[p+1]));
      else Serial.printf("%d,",256*SPIdata[p]+SPIdata[p+1]);
#endif
    }
    Serial.print("\n");
  } while ( checkSumBad);
  // measure WFD times
  wfdTime = millis()-wfdStart;
  WFDmin = _min(WFDmin,wfdTime);
  WFDmax = _max(WFDmax,wfdTime);
  unloadValues();
  dailyEnergy();
  yield(); 
#ifdef RMS1
  batteryEnergy();   // battery simulation only useful on original meter
#endif
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
    if ( checkSumBad ) return false;
    offset = 0;

    Freq = unload2Bytes()/1000.0;
    Vrms = unload2Bytes()/100.0; 
    #ifdef RMS2               
    v = unload2Bytes()/100.0;    
    Vrms_max = _max(Vrms_max,v);
    v = unload2Bytes()/100.0;    
    Vrms_min = _min(Vrms_min,v);
    #endif

    for (uint8_t p=1 ; p<=NUM_CCTS ; p++) { 
      w = unload2Bytes();
      w = (int16_t) w;                // convert unsigned to signed
    //if ( abs(w) < 5.0F ) w = 0.0F;                  // remove low end noise
      Wrms[p] = 0.8*Wrms[p] + 0.2*w;                  // should do smooth over 5 scans
      if (w < Wrms_min[p]) Wrms_min[p] = w;
      if (w > Wrms_max[p]) Wrms_max[p] = w;
    }
    #ifdef RMS1
      avSparekW = 0.99*avSparekW + 0.01*(Wrms[7]-Wrms[1]);
    #endif
    offset = 26;
    v = unload2Bytes()/100.0;    // Vpp_max
    Vmax_p = _max(Vmax_p,v);
    v = unload2Bytes()/100.0;    // Vnp_min
    Vmin_n = _min(Vmin_p,v);
    Vbat = analogRead(A0) * 0.005835;
  }
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

void calcCheckSum() {
  checkSumBad = true;
  checkSum = 0;
  offset = 0;
  while (offset<29) unload2Bytes();  
  uint16_t rxSum = checkSum;                   // sum of bytes 0-29 calc'd by receiver
  uint16_t txSum = (uint16_t) unload2Bytes();  // sum of bytes 0-29 calc'd by transmitter
  if (rxSum == txSum) checkSumBad = false;
  return;   
}