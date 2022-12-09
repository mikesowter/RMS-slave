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

void t31check() {
  const float T31on  = 7.0F; 
  const float T31off = 9.0F; 

  // check battery charge when T31 available (01:00 to 09:00)
  if ( hour() >= 1 && hour() <= 7 ) {
    if ( !T31charging && batt_charge < T31on) {   
      T31charging = true;
      // changeover relay from hotwater to battery charger
    }
    if (T31charging) {            // assume 3.6kW charging
      batt_savings -= 0.0048F;    // T31 cost per minute
      batt_charge += 0.06F;       // kWh charge per minute
      if (batt_charge > T31off) {
        T31charging = false;
        // changeover relay from charger to hotwater
      }
    }
  }
  // else changeover relay from charger to hotwater? every minute?
}