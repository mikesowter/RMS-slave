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
  float v;
  Freq = unload2Bytes()/1000.0;
  Vrms = 0.9*Vrms + unload2Bytes()/1000.0;
  v = unload2Bytes()/50.0;
  Vmin = _min(Vmin,v);
  v = unload2Bytes()/50.0;
  Vmax = _max(Vmax,v);
  for (uint8_t p=0 ; p<10 ; p++) {           // bytes 9-32
  //  Irms[p] = unload2Bytes()/1000.0;
    Wrms[p] = unload2Bytes();
  }
  Serial.printf("\nFreq = %0.3f Vrms=%0.1f Vmin=%0.1f Vmax=%0.1f", Freq, Vrms, Vmin, Vmax);
}

float unload2Bytes() {
  if (offset > 30) Serial.print("illegal SPI data offset ");
  float f = 256.0*(float)SPIdata[offset++];
  f += (float)SPIdata[offset++];
  return f;
}
