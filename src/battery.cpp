/* Battery simulates the existence of 3 levels of Solar panel: 5kW, 7.5kW & 10kW in 
combination with 3 levels of battery capacity: 10kWh, 20kWh, 50kWh
This is required to calculate energy flows between the battery, house and grid on a 
minute by minute basis, and hence the cost of ownership of the offgrid configuration.
*/

#include "extern.h"
#include "defines.h"

float panelCap[] = {5.0F,7.5F,10.0F};
float battCap[] = {16.0F,24.0F,32.0F};
float excessSolar[3], batt_savings[3][3];    // first index is panel size, 2nd is battery size
float batt_tohouse[3][3], batt_charge[3][3], solar_togrid[3][3], batt_togrid[3][3];

extern float noise[];  // 20220725 for oven(6) 
extern float FIT_rate, T11_rate;

void batteryEnergy() {

// general caution - this is a simulation of battery behaviour only
// it does not take account of battery efficiency, inverter losses etc
// all power flows are in kW, energy in kWh, unlike the rest of the code


// sell 2kW to grid between 4pm and 8pm
  float sell2grid = 0.0F;
  float kWms2kWh = (float)t_scan/3.6E6;                         // kWms to kWh (1/1000)*(1/3600)
  if ( hour() >= 16 && hour() < 20 ) sell2grid = 2.0*kWms2kWh;  // 2kW for t_scan ms

  excessSolar[0] = solar-loads;             // excess solar[ps] after house loads in kWh 
  excessSolar[1] = 1.5F*solar-loads; 
  excessSolar[2] = 2.0F*solar-loads;

// first iterate through panels size (ps)
  for (uint8_t ps = 0;ps<3;ps++) {
  // then through battery size (bs)
    for (uint8_t bs = 0;bs<3;bs++) {
      if (excessSolar[ps] > sell2grid) {           
    // +ive is charging battery or solar is exporting to grid  ***********************************************************************
        if (batt_charge[ps][bs] < battCap[bs]) {
          batt_charge[ps][bs] += excessSolar[ps];             // add to battery
          batt_savings[ps][bs] += excessSolar[ps]*FIT_rate;                  //value of energy into battery (is a loss)
          // battery overflows just now
          if ( batt_charge[ps][bs] > battCap[bs]) {
            solar_togrid[ps][bs] += (batt_charge[ps][bs] - battCap[bs]);
            batt_charge[ps][bs] = battCap[bs];
          }
        }
        else {    // battery was full
          solar_togrid[ps][bs] += excessSolar[ps];            // dump all to grid
          batt_savings[ps][bs] += excessSolar[ps] * FIT_rate;                // value of solar energy to grid
        }
      }

      else {                                          
    // -ive excessSolar is discharging battery to house and/or to grid  ***********************************************************************
        if (batt_charge[ps][bs] > battCap[bs]/50.0F) {             // battery > 2% batcap
          if ( sell2grid > 0.0F ) {
            if ( sell2grid > -excessSolar[ps] ) {        // still some grid energy supplied by solar
              float solarShare = sell2grid + excessSolar[ps];     // portion of solar in sell2grid quantity
              solar_togrid[ps][bs] += solarShare;                 // amount of solar sent to grid
              batt_togrid[ps][bs] += sell2grid - solarShare;      // amount of battery sent to grid
              batt_charge[ps][bs] -= sell2grid - solarShare;      // discharging battery
            }
            else {                                       // all grid energy supplied by battery  
              batt_togrid[ps][bs] += sell2grid;
              batt_charge[ps][bs] -= sell2grid;               
            }
          batt_savings[ps][bs] += solar_togrid[ps][bs]*FIT_rate;            // value of solar energy to grid
          batt_savings[ps][bs] += batt_togrid[ps][bs]*FIT_rate;             // value of battery energy to grid
          }  
          else {   // no energy sold to grid, discharge battery to house
            batt_tohouse[ps][bs] -= excessSolar[ps];                       // all battery discharge to house
            batt_savings[ps][bs] -= excessSolar[ps]*FIT_rate;              // value of battery energy to house
            batt_charge[ps][bs] += excessSolar[ps]; ;    
          }
        }
      }   
    }    // end battery size loop
  }   // end panel size loop
}

