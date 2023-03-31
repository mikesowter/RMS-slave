#include <extern.h>

const float chgrEff = 0.95; // charger AC/DC figure, DC/DC better?
const float invtrEff = 0.95;// inverter from battery
const float battCap = 20.0; // kWh figure (from car??)
const float battMin = 1.0;  // don't kill battery  

extern float NOISE[];  // 20220725 for oven(6) 


void batteryEnergy() {

  float batteryFlow = solar-loads;  
  float batteryFlow7_5 = 1.5*solar-loads;  

// first with existing 5kW panels
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
// next with hypothetical 7.5kW panels
  if (batteryFlow7_5 > 0.0) {            // +ive is charging
    if (batt_charge7_5 < battCap) {
      batt_charge7_5 += batteryFlow7_5;   // add to battery
    }
    else {
      batt_togrid7_5 += batteryFlow7_5;   // dump to grid
    }
  }
  else {                                // -ive is discharging
    if (batt_charge7_5 + batteryFlow7_5 > battMin) { 
      batt_charge7_5 += batteryFlow7_5;
      batt_tohouse7_5 -= batteryFlow7_5; 
      batt_savings7_5 = batt_tohouse7_5*(T11-FIT);
    }
    else {    // not enough battery
      batt_charge7_5 = battMin;
    }
  }
}

void t31check() {
  const float T31on  = 3.0F;      // should be based on season
  const float T31off = 5.0F; 

  // check battery charge when T31 available (01:00 to 08:00)
  if ( hour() >= 1 && hour() < 8 ) {
    if ( !T31charging && batt_charge < T31on) {   
      T31charging = true;
      // changeover relay from hotwater to battery charger
    }
    if (T31charging) {            // assume 3.6kW charging
      batt_charge += 0.06F;       // kWh charge per minute
      batt_costs += 0.005F;       // T31 add cost per minute (T11-T31)
      if (batt_charge > T31off) {
        T31charging = false;
        // changeover relay from charger to hotwater
      }
    }
  }
  // else changeover relay from charger to hotwater? if hour==8 && minute==0?
}