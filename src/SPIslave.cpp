#include "extern.h"

float unload2Bytes();
bool unloadValues();
uint16_t checkSum;
float v,w;


void setupSPIslave() {
  // data has been received from the master. len is always 32 bytes
  SPISlave.onData([](uint8_t * data, size_t len) {
  //  SPISlave.setData(data,32);
    for (uint8_t i=0; i<32; i++) MOSIdata[i] = data[i];
    SPIwait = false;
  });

  // The master has read outgoing data buffer
  // that buffer can be set with SPISlave.setData
  SPISlave.onDataSent([]() {  });

  // status has been received from the master.
  // The status register is a special register that both the slave and the master can write to and read from.
  SPISlave.onStatus([](unsigned long data) {
  //  Serial.printf("Master status: %u\n", data);
  });

  // The master has read the status register
  SPISlave.onStatusSent([]() {
  //  Serial.printf("Time code read: %lums\n", millis()%1000);
  });
  // Setup SPI Slave registers and pins
  SPISlave.begin();

  // Set the status register (if the master reads it, it will read this value)
  SPISlave.setStatus(now());
  // Serial.printf("Time code sent: %lums\n", millis()%1000);
}

void waitForData() {
  yield();
  digitalWrite(LED_PIN,0);
  wfdPrev = wfdTime;
  wfdStart = millis();
#ifdef RMS1
  watchWait(60);
#else
  watchWait(350);       // latest master cycle 700ms
#endif

  do {
    SPISlave.begin();
    SPIwait = true;
    while (SPIwait) watchWait(10);
    SPISlave.end(); 
    if (!dataTimeout && millis() > wfdStart + 30000) {
      errMess(" wait for data 30s timeout");
      dataTimeout = true;
      break;  // for minProc
    }
    calcCheckSum();                 // ~350us
    if ( checkSumBad ) MISOdata[0] = 0x15;  // NAK
    else MISOdata[0] = 0x06;        // ACK
    SPISlave.setData(MISOdata,1);   //  ~20us
    } while ( checkSumBad );

    for ( uint8_t p=0;p<32;p+=2 ) {
#ifdef RMS1
      if ( p > 3 && p < 22) Serial.printf("%d,",(int16_t)(256*MOSIdata[p]+MOSIdata[p+1]));
      else Serial.printf("%d,",256*MOSIdata[p]+MOSIdata[p+1]);
#endif
#ifdef RMS2
      if ( p > 7 && p < 22) Serial.printf("%d,",(int16_t)(256*MOSIdata[p]+MOSIdata[p+1]));
      else Serial.printf("%d,",256*MOSIdata[p]+MOSIdata[p+1]);
#endif
    }

  // measure WFD times
  wfdTime = millis()-wfdStart;
  if ( wfdTime < WFDmin ) WFDmin = wfdTime;
  if ( wfdTime > WFDmax ) WFDmax = wfdTime;
  unloadValues();
  dailyEnergy();
  yield(); 
#ifdef RMS1
  batteryEnergy();   // battery simulation only on house dist panel
#endif
  noDataYet = false;   
  digitalWrite(LED_PIN,1);
}

bool unloadValues() {
  Serial.printf("\n%s ",timeStamp());
  if ( MOSIdata[0] == 0xFF && MOSIdata[1] == 0xFF) {
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
    for (int i=FIRST_CCT;i<=NUM_CCTS;i++) {
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

    Freq = 0.8*Freq + 0.2*unload2Bytes()/1000.0;
    Vrms = unload2Bytes()/100.0; 
  #ifdef RMS2       
    Freq *= 2.0;
    v = unload2Bytes()/100.0;    
    Vrms_max = _max(Vrms_max,v);
    v = unload2Bytes()/100.0;    
    Vrms_min = _min(Vrms_min,v);
  #endif

    for (uint8_t cct=1 ; cct<=NUM_CCTS ; cct++) {     // cct 0 is volts
      w = unload2Bytes();
      w = (int16_t) w;    // convert unsigned to signed 
  #ifdef RMS2 
      w = 1.2F * w;       // and scale here temp 12Nov       
  #endif
      if ( abs(w) > 10000.0F || abs(w) < 5.0F ) w = 0.0F;
      Wrms[cct] = 0.5F*w + 0.5F*Wrms[cct];           // remove half the quantizing error   
  #ifdef RMS2
      if ( cct == 7 ) Wrms[7] = 1.056*Wrms[7] - 45.0F; // best guess 18/12/24 was 1.057, -45
  #endif
      if (Wrms[cct] < Wrms_min[cct]) Wrms_min[cct] = Wrms[cct];
      if (Wrms[cct] > Wrms_max[cct]) Wrms_max[cct] = Wrms[cct];
      Wrms_avg[cct] = avgWatts(Wrms[cct],cct,8);
    }
  #ifdef RMS1
      float SparekW = Wrms[7]-Wrms[1];
      avSparekW = 0.99*avSparekW + 0.01*SparekW;
      if ( SparekW > 0.0F ) {
        Wimp = 0.0;
        Wexp = SparekW;
      }
      else {
        Wimp = -SparekW;
        Wexp = 0.0;
      }
      if ( Wrms[5] > 2000.0F ) waterOn = true;
      else waterOn = false;
      for (uint8_t q=NUM_CCTS+1 ; q<=MAX_CCTS ; q++) Wrms[q] = 0.0; // unused inputs
  #else
    
    if ( Wrms_avg[7] > 0.0F ) {   // import power
      Wimp = Wrms_avg[7];
      Wexp = 0.0F;
    }
    else {                        // export power
      Wexp = -Wrms_avg[7];
      Wimp = 0.0F;
    }

    I7phase = Wrms[3];             // phase added 21/11/24
    if ( I7phase > 180.0F ) I7phase -= 360.0F;
  
    offset = 22;
    v = unload2Bytes()/100.0;    // Vpp_max
    Vmax_p = _max(Vmax_p,v);
    v = unload2Bytes()/100.0;    // Vpp_min
    Vmin_p = _min(Vmin_p,v);
  #endif
    offset = 26;
    v = unload2Bytes()/100.0;    // Vnp_max
    Vmax_n = _max(Vmax_n,v);
    v = unload2Bytes()/100.0;    // Vnp_min
    Vmin_n = _min(Vmin_n,v);
    Vbat = analogRead(A0) * 0.006;
  }
  return true;
}

float unload2Bytes() {
  if (offset > 30) {
    diagMess("illegal SPI data offset ");
    return 0.0;
  }
  uint16_t val = 256.0*MOSIdata[offset++];
  val += MOSIdata[offset++];
  checkSum += val;
  return (float)val;
}

void calcCheckSum() {
  checkSum = 0;
  offset = 0;
  while (offset<29) unload2Bytes();  
  uint16_t rxSum = checkSum;                   // sum of bytes 0-29 calc'd by receiver
  uint16_t txSum = (uint16_t) unload2Bytes();  // sum of bytes 0-29 calc'd by transmitter
  if (rxSum == txSum) checkSumBad = false;
  else checkSumBad = true;
  return;   
}