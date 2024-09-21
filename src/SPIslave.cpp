#include "extern.h"

float unload2Bytes();
bool unloadValues();
const float AMPS_CORRECTION_FACTOR = 1.021; // low currents < 4A on 20190922
uint16_t checkSum;
float v,w,w3,w7;

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
  SPISlave.onStatus([](uint32_t data) {
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
  watchWait(90);
#else
  watchWait(400);
#endif

  do {
    SPISlave.begin();
    SPIwait = true;
    while (SPIwait) watchWait(5);
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
    for (int i=1;i<=NUM_CCTS;i++) {
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

    for (uint8_t p=1 ; p<NUM_CCTS ; p++) { 
      w = unload2Bytes();
      w = (int16_t) w;                                // convert unsigned to signed
    //if ( abs(w) < 5.0F ) w = 0.0F;                  // remove low end noise
      Wrms[p] = 0.5*Wrms[p] + 0.5*w;                  // should do smooth over 5 scans
      if (w < Wrms_min[p]) Wrms_min[p] = w;
      if (w > Wrms_max[p]) Wrms_max[p] = w;
    }
    #ifdef RMS1
      avSparekW = 0.99*avSparekW + 0.01*(Wrms[7]-Wrms[1]);
      if ( Wrms[5] > 2000.0F ) waterOn = true;
      else waterOn = false;
      for (uint8_t q=NUM_CCTS+1 ; q<=MAX_CCTS ; q++) Wrms[q] = 0.0; // unused inputs
    #else
    
    // load on main isolator (cct7) if + = export, - = import
    offset = 20;
    w = unload2Bytes();
    w = (int16_t) w; 
    Wrms[7] = 0.45F*Wrms[7] + 0.55F*w;
    w7 = max(0.0F,Wrms[7]);                         
    if (w7 < Wrms_min[7]) Wrms_min[7] = w7;
    if (w7 > Wrms_max[7]) Wrms_max[7] = w7;
    w3 = max(0.0F,-Wrms[7]);
    
  /*  if ( w3 < 10.0F ) {
      sprintf(charBuf,"Wrms[7]=%.0f W7:%.0f W3:%.0f",Wrms[7],w7,w3);
      diagMess(charBuf);
      delay(1000);
    } */
    if (w3 < Wrms_min[3]) Wrms_min[3] = w3;
    if (w3 > Wrms_max[3]) Wrms_max[3] = w3;
        
    Wrms[7] = w7;
    Wrms[3] = w3;

    /* from RMS2 master:
    load2Bytes(Vpp_max*100.0);
    load2Bytes(Vpp_min*100.0);
    load2Bytes(Vnp_max*100.0);
    load2Bytes(Vnp_min*100.0);  */
    
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