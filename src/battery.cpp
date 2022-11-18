#include <extern.h>

const float T31 = 0.166;    // updated 20220901
const float T11 = 0.241;    // updated 20220901
const float FIT = 0.08;     // updated 20211128
const float chgrEff = 0.95; // charger AC/DC figure, DC/DC better?
const float invtrEff = 0.95;// inverter from battery
const float battCap = 20.0; // kWh figure (from car??)
const float battMin = 1.0;  // don't kill battery  

extern float NOISE[];  // 20220725 for oven(6) noise

void batteryEnergy() {

  extern float batt_tohouse, batt_togrid, batt_charge, batt_savings ;
  float batteryFlow = solar-loads;  

  if (batteryFlow > 0.0) {         // +ive is charging
    if (batt_charge < battCap) {
      batt_charge += batteryFlow;   // add to battery
    }
    else {
      batt_togrid += batteryFlow;   // dump to grid
    }
  }
  else {                           // -ive is discharging
    if (batt_charge + batteryFlow > battMin) { // enough battery
      batt_charge += batteryFlow;
      batt_tohouse -= batteryFlow; 
      batt_savings = batt_tohouse*(T11-FIT);
    }
    else {    // not enough battery
      batt_charge = battMin;
    }
  }
}
