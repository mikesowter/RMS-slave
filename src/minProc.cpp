
#include "extern.h"

uint8_t en_index, en5index, en15index;
double Imp_kWh_now;
void writePeak();
void fillBarrel();

// scheduled processing

void minProc() {
  // update master time
  SPISlave.setStatus(now());
  if ( minute() == oldMin ) return;
  oldMin = minute();
  if ( old5Min == minute()/5 ) return;
  old5Min = minute()/5;
  // energy calcs every 5 minutes
  en_index = (minute()/5)%6;          // current index into 6 x 5 min energy readings
  en5index = (en_index+5)%6;          // index 5 mins back
  en15index = (en_index+3)%6;         // index 15 mins back
#ifdef RMS1
  Imp_kWh_now = T11_kWh[0];
  rms15Demand = (Imp_kWh_now - Imp_5m_kWh[en15index])*4000.0F;
  rms30Demand = (Imp_kWh_now - Imp_5m_kWh[en_index])*2000.0F;  
  Imp_5m_kWh[en_index] = Imp_kWh_now;     // overwrite value from 30m ago

#else
  rms5Demand  = (Imp_meter - Imp_5m_kWh[en5index])*12000.0F;
  rms15Demand = (Imp_meter - Imp_5m_kWh[en15index])*4000.0F;
  rms30Demand = (Imp_meter - Imp_5m_kWh[en_index])*2000.0F;  
  if ( rms5Demand > 10000.0F || rms15Demand > 10000.0F || rms30Demand > 10000.0F ) {
//    sprintf(charBuf,"now:%.3f 0:%.3f 1:%.3f 2:%.3f 3:%.3f 4:%.3f 5:%.3f",Imp_meter,
//              Imp_5m_kWh[0],Imp_5m_kWh[1],Imp_5m_kWh[2],Imp_5m_kWh[3],Imp_5m_kWh[4],Imp_5m_kWh[5]);
//   diagMess(charBuf);
    rms5Demand = 0.0F;
    rms15Demand = 0.0F;
    rms30Demand = 0.0F;         
  }
  Imp_5m_kWh[en_index] = Imp_meter;     // overwrite value from 30m ago
#endif
  if ( peakPeriod ) {
    if ( rms15Demand > rms15Peak ) rms15Peak = rms15Demand;
    if ( rms30Demand > rms30Peak ) rms30Peak = rms30Demand;
    writeDemands();
  }

  // check for new quarter hour
  if ( oldQtr == minute()/15 ) return;
  oldQtr = minute()/15;
  storeData();                        // write day file every 15mins
  if ( hour() == 16 && oldQtr == 1 ) peakPeriod = true;
  if ( oldHour == hour() ) return;
  oldHour = hour();
  if ( hour() == 21 ) {
    writePeak();
    peakPeriod = false;
  }
  
  // check for end of day
  if ( day() == oldDay ) return;
  // update time at 00:00:00
  setupTime();
  // write days energy totals
  updateEnergyFile();
#ifdef RMS1
  // battery simulation totals
  updateBatteryFile();
  // iterate through panel size (ps)
  for (uint8_t ps = 0;ps<3;ps++) {
    // then through battery size (bs)
    for (uint8_t bs = 0;bs<3;bs++) {
      batt_tohouse[ps][bs] = 0.0F;
      dump_togrid[ps][bs] = 0.0F;   
      batt_savings[ps][bs] = 0.0F;
    }
    T11_kWh[ps] = 0.0F;
  }
#endif
  rms15Peak = 0.0F;
  rms30Peak = 0.0F;

  // reset daily energy sums at midnight
  for ( int i = 0; i<NUM_CCTS+1; i++ ) {
    Energy[i] = 0.0F;
    costEnergy[0][i] = 0.0F;
    costEnergy[1][i] = 0.0F;
    costEnergy[2][i] = 0.0F;
  }
}  
