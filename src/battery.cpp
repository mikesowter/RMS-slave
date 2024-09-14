/* Battery simulates the existence of 3 levels of Solar panel: 5kW, 7.5kW & 10kW in 
combination with 3 levels of battery capacity: 10kWh, 20kWh, 50kWh
This is required to calculate energy flows between the battery, house and grid on a 
minute by minute basis, and hence the cost of ownership of the offgrid configuration.
*/

#include "extern.h"
#include "defines.h"

float panelCap[] = {5.0F,7.5F,10.0F};
float battCap[] = {10.0F,20.0F,50.0F};
float excessSolar[3], batt_savings[3][3];    // first index is panel size, 2nd is battery size
float batt_tohouse[3][3], batt_charge[3][3], dump_togrid[3][3];

extern float noise[];  // 20220725 for oven(6) 

void batteryEnergy() {

  excessSolar[0] = solar-loads;  
  excessSolar[1] = 1.5F*solar-loads;  
  excessSolar[2] = 2.0F*solar-loads;  

// first iterate through panels size (ps)
  for (uint8_t ps = 0;ps<3;ps++) {
  // then through battery size (bs)
    for (uint8_t bs = 0;bs<3;bs++) {
      if (excessSolar[ps] > 0.0) {                  // +ive is charging
        if (batt_charge[ps][bs] < battCap[bs]) {
          batt_charge[ps][bs] += excessSolar[ps];   // add to battery
          if ( batt_charge[ps][bs] > battCap[bs]) {
            dump_togrid[ps][bs] += (batt_charge[ps][bs] - battCap[bs]);
            batt_charge[ps][bs] = battCap[bs];
          }
        }
        else {
          dump_togrid[ps][bs] += excessSolar[ps];   // dump to grid
        }
      }
      else {                                          // -ive is discharging
        float battMin = battCap[bs]/50.0F;
        if (batt_charge[ps][bs] > battMin) {          // battery > 2% batcap
          batt_charge[ps][bs] += excessSolar[ps];
          if ( batt_charge[ps][bs] < battMin) {       // marginally too low
          batt_tohouse[ps][bs] += (batt_charge[ps][bs] - battMin); 
          batt_charge[ps][bs] = battMin;              // battery at minimum
          }
          else batt_tohouse[ps][bs] -= excessSolar[ps]; 
        }
        else {    // not enough battery
          batt_charge[ps][bs] = battMin;
        }
      }
      batt_savings[ps][bs] = batt_tohouse[ps][bs]*T11;                
      batt_savings[ps][bs] += dump_togrid[ps][bs]*FIT-costEnergy[ps][7];  
    }
  }
}

