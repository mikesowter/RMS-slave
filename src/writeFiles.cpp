#include "extern.h"
extern float T11_kWh[];           // daily sum from grid with each panel size
extern float T11_inc[];           // increment from grid

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
  fh.print(Imp_meter);
  fh.print(",");
  fh.println(Exp_meter);
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
#ifdef RMS1
  for ( int i=FIRST_CCT; i<NUM_CCTS+1; i++ ) {
    fh.printf(",%.2f",Energy[i]);
    fh.printf(",$%.2f",costEnergy[0][i]);
    fh.printf(",$%.2f",costEnergy[1][i]);
    fh.printf(",$%.2f",costEnergy[2][i]);
  }
  fh.printf(",%.2f,%.2f,%.2f",T11_kWh[0],T11_kWh[1],T11_kWh[2]);
#else
  for ( int i=3; i<NUM_CCTS+1; i++ ) {
    fh.printf(",%.2f",Energy[i]);
  }
#endif
  fh.close();
}

void updateBatteryFile() {
  if (!LittleFS.exists("/Battery.csv")) {
    fh = LittleFS.open("/Battery.csv", "w");
    if (!fh) {
      diagMess("Battery.csv failed to create");
      return;
    }
    fh.printf("Date,Panel Size,Batt Size,Charge (kWh),To House (kWh),To Grid (kWh),Savings ($),");
    fh.printf("Panel Size,Batt Size,Charge (kWh),To House (kWh),To Grid (kWh),Savings ($),");
    fh.printf("Panel Size,Batt Size,Charge (kWh),To House (kWh),To Grid (kWh),Savings ($),\n");
    fh.close();
  }
  fh = LittleFS.open("/Battery.csv", "a");
  if (!fh) {
    diagMess("Battery.csv failed to open");
    return;
  }
  // write daily battery simulation results
  fh.printf("\n");
  fh.printf("%02d/%02d/%4d,",day(),month(),year());
  for (uint8_t ps = 0;ps<3;ps++) {
    fh.printf(" PS%d,",ps);
    for (uint8_t bs = 0;bs<3;bs++) {
      fh.printf(" BS%d,%.2f,%.2f,%.2f,%.2f,",
             bs,batt_charge[ps][bs], batt_tohouse[ps][bs], solar_togrid[ps][bs], batt_savings[ps][bs]); 
    }
  }
  fh.close();
}

void writePeak() {
  fh = LittleFS.open("PeakDemand.csv","a+");
  fh.printf("%s,%.0f,%.0f\n",dateStamp(),rms15Peak,rms30Peak);
  fh.close();
  return;
}

void writeDemands() {
  char name[20];
  strcpy(name,"DM");
  strcat(name,dateStamp());
  strcat(name,".csv");
  fh = LittleFS.open(name,"a+");
  fh.printf("%s,%.0f,%.0f,%.0f\n",timeStamp(),rms5Demand,rms15Demand,rms30Demand);
  fh.close();
  return;
}