#include "extern.h"

byte openFile(const char* s);
void WriteQtr();

byte storeData() {
  strcpy(fileName,todayName);
  if ( !openFile("a") ) return 0;
  WriteQtr();
  fh.close();
  return 1;
}

//----------------- open file for reading or appending

byte openFile(const char* s) {
  fh = LittleFS.open(fileName, s);
  if (!fh) {
    strcpy(charBuf,fileName);
    strcat(charBuf," failed to open");
    diagMess(charBuf);
    return 0;
  }
  return 1;
}

void WriteQtr() {
  fh.print(i2sd(hour()));
  fh.print(":");
  fh.print(i2sd(minute()));
  fh.print(",");
  fh.print(Vrms);
  fh.print(",");
  fh.print(Energy[1]);
  fh.print(",");
  fh.println(T11_kWh);
  delay(10);
}

void updateEnergyFile() {
  fh = LittleFS.open("/Energy.csv", "a");
  if (!fh) {
    diagMess("Energy.csv failed to open");
    return;
  }
  // write daily energy sums
  fh.printf("\n%02d/%02d/%4d",day(),month(),year());
  for ( int i=1; i<NUM_CIRCUITS+1; i++ ) {
    fh.printf(",%.2f",Energy[i]);
    fh.printf(",$%.2f",costEnergy[i]);
  }
  fh.printf(",%.2f,%.2f,%.2f",T11_kWh,T11_kWh7_5,T11_kWh10);
  fh.close();
}

void updateBatteryFile() {
  fh = LittleFS.open("/Battery.csv", "a");
  if (!fh) {
    diagMess("Battery.csv failed to open");
    return;
  }
  // write daily battery simulation results
  fh.printf("\n%02d/%02d/%4d,",day(),month(),year());
  fh.printf("%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f",
             batt_charge, batt_tohouse, batt_togrid, batt_savings, batt_costs,
             batt_charge7_5,batt_tohouse7_5,batt_togrid7_5,batt_savings7_5,
             batt_charge10,batt_tohouse10,batt_togrid10,batt_savings10,
             T11_kWh,T11_kWh7_5,T11_kWh10);
  fh.close();
}
