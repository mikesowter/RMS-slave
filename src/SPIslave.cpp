#include "extern.h"

float unload2Bytes();
void unloadValues();
const float AMPS_CORRECTION_FACTOR = 1.021; // low currents < 4A on 20190922


void setupSPIslave() {
  // data has been received from the master. len is always 32 bytes
  SPISlave.onData([](uint8_t * data, size_t len) {
    SPISlave.setData(data,32);
    for (uint8_t i=0; i<32; i++) SPIdata[i] = data[i];
    SPIwait = false;
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
  SPISlave.begin();
  while (SPIwait) watchWait(1);
  SPIwait = true;
  SPISlave.end(); 
  unloadValues();
  yield();
  dailyEnergy();
  yield(); 
  batteryEnergy();      
  noDataYet = false;   
}

void unloadValues() {
  if ( SPIdata[0] == 0xFF && SPIdata[1] == 0xFF) {
    if ( !pwrOutage ) {
      pwrOutage = true;
      diagMess("power outage start");
    }
    Freq = 0.0;
    Vrms = 0.0;           
    Vrms_min = 0.0;      
    Vrms_max = 0.0;      
    Vmin_n = 0.0;      
    Vmax_n = 0.0;      
    Vmin_p = 0.0;      
    Vmax_p = 0.0; 
    offset = 30;
    Vbat = unload2Bytes()/1437.7F;
  }
  else {
    if ( pwrOutage ) {
      pwrOutage = false;
      diagMess("power restored");
    }
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
    exporting = ( Wrms[7] > Wrms[1] );             // solar > local usage
    exporting7_5 = ( Wrms[7]*1.5F > Wrms[1] );     // sim solar > local usage
    exporting10 = ( Wrms[7]*2.0F > Wrms[1] );      // sim solar > local usage
    // for big load queries (time to start/stop the pool heater?)
    avSparekW = 0.99*avSparekW + 0.01*( Wrms[7] - Wrms[1] );  

  }
  offset = 30;
  Vbat = unload2Bytes()/1437.7F;
}

float unload2Bytes() {
  if (offset > 31) diagMess("illegal SPI data offset ");
  float f = 256.0*(float)SPIdata[offset++];
  f += (float)SPIdata[offset++];
  return f;
}
