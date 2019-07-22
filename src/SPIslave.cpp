#include "extern.h"

float unload2Bytes();
void unloadValues();


void setupSPIslave() {
  // data has been received from the master. len is always 32 bytes
  SPISlave.onData([](uint8_t * data, size_t len) {
    SPISlave.setData(data,32);
    for (uint8_t i=0;i<32;i++) SPIdata[i] = data[i];
    noData = false;
  });

  // The master has read out outgoing data buffer
  // that buffer can be set with SPISlave.setData
  SPISlave.onDataSent([]() {
    Serial.println(" *");
   });

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
  uint32_t t2 = millis();
  SPISlave.begin();
  while (noData) watchWait(10);
  noData = true;
  delay(10);        
  SPISlave.end(); 
  /* Serial.println();
  for (uint8_t i=0 ; i<32 ; i++) {
    Serial.print(SPIdata[i],HEX);
    Serial.print(" ");
  }  */
  Serial.println();
  Serial.print(timeStamp());
  Serial.printf("  waited %ld ms  ",millis()-t2);
  unloadValues();
  delay(10);
}

void unloadValues() {
  offset = 0;
  float v,w;
  Freq = unload2Bytes()/1000.0;
  Vrms = unload2Bytes()/100.0;
  Vrms_min = _min(Vrms_min,Vrms);
  Vrms_max = _max(Vrms_max,Vrms);
  v = unload2Bytes()/50.0;
  Vmin = _min(Vmin,-v);
  Vmin_15 = _min(Vmin_15,Vmin );
  v = unload2Bytes()/50.0;
  Vmax = _max(Vmax,v);
  Vmax_15 = _min(Vmax_15,Vmax );
  Serial.printf("Freq = %0.3f Vrms=%0.1f Vmin=%0.1f Vmax=%0.1f\n", Freq, Vrms, Vmin, Vmax);

  for (uint8_t p=1 ; p<(NUM_CHANNELS+1) ; p++) {           // bytes 9-32
    Wrms[p] = unload2Bytes();
    Wrms_min[p] = _min(Wrms_min[p],Wrms[p]);
    Wrms_max[p] = _max(Wrms_max[p],Wrms[p]);
    Serial.printf("W[%i] = %.0f,%.0f ",p,Wrms_min[p],Wrms_max[p]);
    if (Wrms_max[p] > 20000) {
      sprintf(charBuf,"Wrms_max[%i] = %.0f\n",p,Wrms_max[p]);
      diagMess(charBuf);
      Wrms_max[p] = 0.0;
    }
  }

}

float unload2Bytes() {
  if (offset > 30) Serial.print("illegal SPI data offset ");
  float f = 256.0*(float)SPIdata[offset++];
  f += (float)SPIdata[offset++];
  return f;
}
