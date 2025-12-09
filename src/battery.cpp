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
extern float spotPrice, amberPrice;

void batteryEnergy() {

// general caution - this is a simulation of battery behaviour only
// it does not take account of battery efficiency, inverter losses etc
// all power flows are in kW, energy in kWh, unlike the rest of the code


// sell 2kW to grid if spot price > threshold
  float sell2grid = 0.0F;
  float kWus2kWh = (float)t_scan/3.6E9;                         // kWus to kWh (1/E6)*(1/3600)
  float spotPrice_kWh = spotPrice/1000.0F;                      // $/MWh to $/kWh
  bool spareCap;

  excessSolar[0] = solar-loads;             // excess solar[ps] after house loads in kWh 
  excessSolar[1] = 1.5F*solar-loads; 
  excessSolar[2] = 2.0F*solar-loads;

// first iterate through panels size (ps)
  for (uint8_t ps = 0;ps<3;ps++) {
  // then through battery size (bs)
    for (uint8_t bs = 0;bs<3;bs++) {
      if ( spotPrice > 100.0F ) sell2grid = battCap[bs]/8.0F*kWus2kWh;  // C/8 rate for t_scan us
      else sell2grid = 0.0F;
      spareCap = ( batt_charge[ps][bs] > battCap[bs]/4.0F );            // at least 25% charge available?

      if ( sell2grid > 0.0F && spareCap) {                              // selling to grid, do not charge battery
        if ( excessSolar[ps] > sell2grid ) {
          solar_togrid[ps][bs] += excessSolar[ps];                      // amount of solar sent to grid
          batt_savings[ps][bs] += excessSolar[ps] * spotPrice_kWh;        // value of solar energy to grid  
        }
        else {
          solar_togrid[ps][bs] += max(0.0F,excessSolar[ps]);             // amount of solar sent to grid
          float fromBatt = max(0.0F,sell2grid - excessSolar[ps]);
          batt_togrid[ps][bs] += fromBatt;                               // amount of battery sent to grid
          batt_tohouse[ps][bs] += loads;                                 // amount of battery sent to house
          batt_charge[ps][bs] -= fromBatt + loads;
          batt_savings[ps][bs] += (fromBatt + loads) * amberPrice;       // value of energy from battery
        }
      }
      else {    // not selling to grid FROM BATTERY
        if ( excessSolar[ps] > 0.0F ) {                       // charge battery if possible
          if (batt_charge[ps][bs] > battCap[bs]) {            // battery full
            solar_togrid[ps][bs] += excessSolar[ps];          // send solar to grid
          }
          else if ( spotPrice_kWh < .02F) {                   // battery not full and energy cheap
            batt_charge[ps][bs] += excessSolar[ps];           // add to battery
            batt_savings[ps][bs] -= excessSolar[ps] * spotPrice_kWh;   // value of energy into battery (is a loss)
          }
          else {                                              // battery not full but energy expensive
            solar_togrid[ps][bs] += excessSolar[ps];          // send solar to grid
            batt_savings[ps][bs] -= excessSolar[ps] * spotPrice_kWh;   // value of energy
          }
        }
        else {      // no solar
          if (batt_charge[ps][bs] > battCap[bs]/50.0F) {       // discharge battery if possible
            batt_charge[ps][bs] += excessSolar[ps];            // take from battery
            batt_tohouse[ps][bs] -= excessSolar[ps];              
            batt_savings[ps][bs] -= excessSolar[ps] * amberPrice;   // value of energy from battery
          }
        }   // end solar available
      }   // end selling to grid
    }   // end battery size loop  
  }   // end panel size loop
}
