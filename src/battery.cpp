/* Battery simulates the existence of 3 levels of Solar panel: 5kW, 7.5kW & 10kW in 
combination with 3 levels of battery capacity: 10kWh, 20kWh, 50kWh
This is required to calculate energy flows between the battery, house and grid on a 
minute by minute basis, and hence the payback period for the battery installation.
Battery flows to grid are costed at the spot price at the time of export, and 
battery flows to house are costed at the amber rate avoided.
The battery is charged from excess solar only when the spot price is below 5c/kWh, 
and battery is discharged to the grid when the spot price is above 12c/kWh.
The rate of discharge to the grid is limited to C/8 of the battery capacity.
Power is only sold to the grid when the battery has at least 25% charge available.
*/

#include "extern.h"
#include "defines.h"

float panelCap[] = {5.0F,7.5F,10.0F};
float battCap[] = {16.0F,24.0F,32.0F};
float excessSolar[3], batt_savings[3][3];    // first index is panel size, 2nd is battery size
float batt_tohouse[3][3], batt_charge[3][3], solar_togrid[3][3], batt_togrid[3][3];
float sell2grid;
float micros2hrs = (float)t_scan/3.6E9;                       // (1/E6)*(1/3600)
uint8_t dawnByMonth[12] = {5,6,6,7,7,8,8,7,7,6,6,5};            // approximate dawn hour by month
uint8_t firstLight = dawnByMonth[month()-1];
float sellGridRate;    // kW rate of selling to grid depending on spot price
float buyGridRate;     // kW rate of buying from grid depending on state of battery


extern float noise[];  // 20220725 for oven(6) 
extern float spotPrice, amberPrice;

bool decideToSell(float battLevel);
float buy(float battLevel, float battCapacity);

void batteryEnergy() {
// general caution - this is a simulation of battery behaviour only
// it does not take account of battery efficiency, inverter losses etc
// all power flows are in kW, energy in kWh, unlike the rest of the code (in W)


  float spotPrice_kWh = spotPrice/1000.0F;                      // $/MWh to $/kWh

  excessSolar[0] = solar-loads;             // excess solar[ps] after house loads in kWh 
  excessSolar[1] = 1.5F*solar-loads; 
  excessSolar[2] = 2.0F*solar-loads;

// first iterate through panels size (ps)
  for (uint8_t ps = 0;ps<3;ps++) {
  // then through battery size (bs)
    for (uint8_t bs = 0;bs<3;bs++) {
      if ( decideToSell( batt_charge[ps][bs] )) {                      // selling to grid, do not charge battery
        if ( excessSolar[ps] > sell2grid ) {    // all from solar
          solar_togrid[ps][bs] += excessSolar[ps];                      // amount of solar sent to grid
        }
        else {                                  // some from battery
          float fromSolar = max(0.0F,excessSolar[ps]);  
          solar_togrid[ps][bs] += fromSolar;                             // amount of solar sent to grid
          float fromBatt = max(0.0F,sell2grid - fromSolar);
          batt_togrid[ps][bs] += fromBatt;                               // amount of battery sent to grid
          batt_tohouse[ps][bs] += loads;                                 // amount of battery sent to house
          batt_charge[ps][bs] -= fromBatt + loads;
          batt_savings[ps][bs] += loads * amberPrice;                    // money saved
          batt_savings[ps][bs] += fromBatt * spotPrice_kWh;              // money earned
        }
      }
      else {    // not selling to grid for price reasons
        if ( excessSolar[ps] > 0.0F ) {                       // charge battery if possible
          if (batt_charge[ps][bs] > battCap[bs]) {            // battery full
            solar_togrid[ps][bs] += excessSolar[ps];          // send solar to grid
          }
          else {     
            batt_charge[ps][bs] += excessSolar[ps];           // add to battery
            batt_savings[ps][bs] -= excessSolar[ps] * spotPrice_kWh;   // value of energy into battery (is a loss)
          }
          float buyFromGrid = buy(batt_charge[ps][bs],battCap[bs]);    // based on price, charge and time of day   
          if ( buyFromGrid > 0.0F ) {
            batt_charge[ps][bs] += buyFromGrid;                        // add to battery
            batt_savings[ps][bs] -= buyFromGrid * amberPrice;          // value of energy into battery (is a loss) 
          }                                     
        }
        else {      // no solar, all from battery if available
          if (batt_charge[ps][bs] > battCap[bs]/50.0F) {       // discharge battery to 2% capacity
            batt_charge[ps][bs] += excessSolar[ps];            // take from battery (excess is negative)
            batt_tohouse[ps][bs] -= excessSolar[ps];              
            batt_savings[ps][bs] -= excessSolar[ps] * amberPrice;   // money saved
          }
        }   // end solar available
      }   // end selling to grid
    }   // end battery size loop  
  }   // end panel size loop
}

bool decideToSell(float battLevel) {
  // decide whether to sell to grid based on battery level and time of day

  // first priority is to have enough battery for the night
  if ( hour() < firstLight || hour() > (24 - firstLight) ) {      // if night time
    uint8_t hrsToDawn = (24 - hour() + firstLight) % 24;
    if ( battLevel < 0.7F * hrsToDawn ) return false;             // do not sell if battery will not last till dawn
  }
  // second priority is how much to sell during peak spot prices
  sellGridRate = max(0.0F,min(5.0F,(spotPrice-120.0F)/100.0F));  // up to 5kW depending on price 12c-62c/kWh
  sell2grid = sellGridRate*micros2hrs;                           // power * time = energy in kWh
  if ( sell2grid > 0.0F ) return true;      
  sell2grid = 0.0F;                      
  return false;
}

float buy(float battLevel, float battCapacity) {
  // decide whether to buy based on battery level and time of day
  uint8_t hrsFromDawn = hour() - firstLight;
  uint8_t hrsInDay = 24 - 2*firstLight;
  if ( hour() >= firstLight && hour() <= (24 - firstLight) ) {            // if daytime
    if ( battCapacity/battLevel < hrsFromDawn/hrsInDay ) {                // buy if battery will not fill by dusk
      buyGridRate = max(0.0F,min(5.0F,(50.0F-spotPrice)/10.0F));          // up to 5kW rate depending on price
      return buyGridRate * micros2hrs;                                    // power * time = energy in kWh
    }
  }       
  return 0.0F;       
}
