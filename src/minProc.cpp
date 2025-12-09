
#include "extern.h"
extern float spotPrice, amberPrice;
float readPromItem(char* unit);
char AEMOSpotPrice[] = "AEMOSpotPrice";
char AEMOamberImpCost[] = "AEMOamberImpCost";

uint8_t en_index, en5index, en15index;
double Imp_kWh_now;
void writePeak();
// void fillBarrel();

// scheduled processing

void minProc() {
  // update master time
  SPISlave.setStatus(now());
  if ( minute() == oldMin ) return;
  oldMin = minute();
  spotPrice = readPromItem(AEMOSpotPrice);
  amberPrice = readPromItem(AEMOamberImpCost);

  // check for new 5 minute period
  if ( old5Min == minute()/5 ) return;
  old5Min = minute()/5;
    
  // check for new quarter hour
  if ( oldQtr == minute()/15 ) return;
  oldQtr = minute()/15;
  storeData();                        // write day file every 15mins
  if ( hour() == 16 ) peakPeriod = true;
  if ( oldHour == hour() ) return;
  oldHour = hour();
  if ( hour() == 20 ) {
//    writePeak();
    peakPeriod = false;
  }
  
  // check for end of day
  if ( day() == oldDay ) return;
  // write days energy totals
  write24hEnergy();
#ifdef RMS1
  // battery simulation totals
  updateBatteryFile();
  // reset battery sums at midnight
  // iterate through panel size (ps)
  for (uint8_t ps = 0;ps<3;ps++) {
    // then through battery size (bs)
    for (uint8_t bs = 0;bs<3;bs++) {
      batt_tohouse[ps][bs] = 0.0F;
      solar_togrid[ps][bs] = 0.0F;   
      batt_savings[ps][bs] = 0.0F;
      batt_togrid[ps][bs] = 0.0F;
    }
    T11_kWh[ps] = 0.0F;
  }
#endif
//  rms15Peak = 0.0F;     removed 20241002 as demand analysis not required
//  rms30Peak = 0.0F;

  // reset daily energy sums at midnight
  for ( int i = 0; i<NUM_CCTS+1; i++ ) {
    Energy[i] = 0.0F;
    costEnergy[0][i] = 0.0F;
    costEnergy[1][i] = 0.0F;
    costEnergy[2][i] = 0.0F;
  }
  // update time at 00:00:00
  setupTime();
  t_lastData = micros();    // avoid energy bump at midnight
}  
