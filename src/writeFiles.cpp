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
  fh.printf("\n%s,%s",dateStamp(),timeStamp());
  for ( int i=1; i<NUM_CIRCUITS+1; i++ ) {
    fh.printf(",%7.4f",Energy[i]);
    fh.printf(",$%.2f",costEnergy[i]);
  }
  fh.printf(",%.2f",T11_kWh);
  fh.close();
}

void updateBatteryFile() {
  fh = LittleFS.open("/Battery.csv", "a");
  if (!fh) {
    diagMess("Battery.csv failed to open");
    return;
  }
  // write daily battery simulation results
  fh.printf("\n%s,%s",dateStamp(),timeStamp());
  fh.printf("%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f",
             batt_charge, batt_tohouse, batt_togrid, batt_savings, batt_costs,
             batt_charge75,batt_tohouse75,batt_togrid75,batt_savings75,batt_costs75);
  fh.close();
}
