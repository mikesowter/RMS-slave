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

  float spareSolar, split, rate;
  extern float T11_kWh, T11_inc, T11_batt, battExport, battEnergy ;

  t_scan = millis() - t_lastData;
  t_lastData = millis();

   for ( int i = 1;i<NUM_CIRCUITS+1;i++ ) {
    if ( Wrms[i] < NOISE[i] ) Wrms[i] = 0.0;    // eliminate noise
    incEnergy[i] = Wrms[i]*(float)t_scan/3.6e9; // kWh units
   }

  spareSolar = incEnergy[7] - incEnergy[1];

  if (spareSolar > 0.0) {    
    if (battEnergy < battCap) {
      battEnergy += spareSolar;   // add to battery
    }
    else {
      battExport += spareSolar;   // dump to grid
    }
  }
  else {      // spareSolar is negative
    if (battEnergy + spareSolar > battMin) { // enough battery
      battEnergy += spareSolar;
      T11_batt -= spareSolar; 
    }
    else {    // not enough battery
      battEnergy = battMin;
    }
  }
}
