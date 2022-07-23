#include "extern.h"

float unload2Bytes();
void unloadValues();
const float AMPS_CORRECTION_FACTOR = 1.021; // low currents < 4A on 20190922


void setupSPIslave() {
  // data has been received from the master. len is always 32 bytes
  SPISlave.onData([](uint8_t * data, size_t len) {
    SPISlave.setData(data,32);
    for (uint8_t i=0; i<32; i++) SPIdata[i] = data[i];
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
  uint32_t w = millis();
  SPISlave.begin();
  while (noData) watchWait(1);
  waiting = millis() - w;
  if ( waiting < WDmin) WDmin = waiting;
  if ( waiting > WDmax) WDmax = waiting;
  noData = true;
  SPISlave.end(); 
  unloadValues();
  yield();
  dailyEnergy();
  yield();            
}

void unloadValues() {
  offset = 0;
  float v,w;
  Freq = unload2Bytes()/1000.0;
  if (Freq < 40.0 || Freq > 55.0 ) Freq = 50.0;   // remove freq errors from record
  Vrms = unload2Bytes()/100.0;                
  Vrms_min = _min(Vrms_min,Vrms);
  Vrms_max = _max(Vrms_max,Vrms);
  v = unload2Bytes()/50.0;    // peak from negative half cycle
  Vmin = _min(Vmin,v);
  Vmax = _max(Vmax,v);
  v = unload2Bytes()/50.0;    // peak from positive half cycle
  Vmin = _min(Vmin,v);
  Vmax = _max(Vmax,v);
  // Serial.printf("Freq = %0.3f Vrms=%0.1f Vmin=%0.1f Vmax=%0.1f\n", Freq, Vrms, Vmin, Vmax);

  if ( Vmin > -400.0 && Vmax < 400.0) {           // remove spurious surge power from the record
    for (uint8_t p=1 ; p<(NUM_CIRCUITS+1) ; p++) { 
      w = unload2Bytes();
      if ( w > 15000 ) w = 4999;                  // reasonability limit
      else if ( w < 5 ) w = 0.0;                  // remove low end noise
      Wrms[p] = (float)w;    
      Wrms_min[p] = _min( Wrms_min[p], w );
      Wrms_max[p] = _max( Wrms_max[p], w );
      // Serial.printf("W[%i] = %.0f,%.0f ",p,Wrms_min[p],Wrms_max[p]);
    }
  }
//  waterOn = ( Wrms[5] > 1000 );           // hot water is channel 5
  waterOn = false;
  if ( Wrms[5] > 1000 ) waterOn = true;
  exporting = ( Wrms[7] > Wrms[1] );      // solar > usage

  offset = 30;
  Vbat = unload2Bytes()/1437.7;
}

float unload2Bytes() {
  if (offset > 31) diagMess("illegal SPI data offset ");
  float f = 256.0*(float)SPIdata[offset++];
  f += (float)SPIdata[offset++];
  return f;
}
