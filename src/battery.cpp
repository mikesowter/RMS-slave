/* Battery simulates the existence of 3 levels of Solar panel: 5kW, 7.5kW & 10kW in 
combination with 3 levels of battery capacity: 10kWh, 20kWh, 50kWh
This is required to calculate energy flows between the battery, house and grid on a 
minute by minute basis, and hence the payback period for the battery installation.
Battery flows to grid are costed at the spot price at the time of export, and 
battery flows to house are costed at the amber rate avoided.
The simulation is basic and does not take account of battery efficiency, inverter losses etc.
*/

#include "extern.h"
#include "defines.h"

float panelCap[] = {5.0F,7.5F,10.0F};
float battCap[] = {16.0F,24.0F,32.0F};
float excessSolar[3], batt_savings[3][3];    // first index is panel size, 2nd is battery size
float batt_tohouse[3][3], batt_charge[3][3], solar_togrid[3][3], batt_togrid[3][3];
float sellToGrid, buyFromGrid;
float micros2hrs = 1/3.6E9;                                       // (1/E6)*(1/3600)
uint8_t dawnByMonth[12] = {6,6,7,7,8,8,8,8,7,7,6,6};              // 1kW dawn hour by month
uint8_t firstPVkW = dawnByMonth[month()-1];
float sellGridRate;    // kW rate of selling to grid depending on spot price
float buyGridRate;     // kW rate of buying from grid depending on state of battery


extern float noise[];  // 20220725 for oven(6) 
extern float spotPrice, amberPrice;

bool decideToSell(float battLevel);
bool decideToBuy(float battLevel, float battCapacity);

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
        if ( excessSolar[ps] > sellToGrid ) {    // all from solar
          solar_togrid[ps][bs] += excessSolar[ps];                      // amount of solar sent to grid
        }
        else {                                  // some from battery
          float fromSolar = max(0.0F,excessSolar[ps]);  
          solar_togrid[ps][bs] += fromSolar;                             // amount of solar sent to grid
          float fromBatt = max(0.0F,sellToGrid - fromSolar);
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
          if ( decideToBuy(batt_charge[ps][bs],battCap[bs]) ) {
            batt_charge[ps][bs] += buyFromGrid;                        // add to battery
            batt_savings[ps][bs] -= buyFromGrid * amberPrice;          // value of energy into battery (is a loss) 
          }                                     
        }
        else {      // no solar, all from battery if available
          if (batt_charge[ps][bs] > battCap[bs]*0.05F) {       // discharge battery to 5% capacity
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
  // first priority is to handle very high spot prices
  if ( spotPrice > 1000.0F) {                                     // over $1.00/kWh  ($10000/MWh)
    sellGridRate = 10000.0F;                                      // sell at 10kW rate (if possible)
    sellToGrid = sellGridRate * micros2hrs * t_scan;              // power * time = energy in Wh
    return true;
  }
  // second priority is to have enough battery for the night
  uint8_t lastPVkW = (23 - firstPVkW);                            // solar midday = 11:30
  if ( hour() < firstPVkW || hour() > lastPVkW ) {                // if night time
    uint8_t hrsToDawn = (24 - hour() + firstPVkW) % 24;
    if ( battLevel < 700.0F * hrsToDawn ) return false;           // only sell if battery will last till dawn
  }
  // third priority is how much to sell during normal high spot prices
  sellGridRate = max(0.0F,min(5000.0F,spotPrice*10.0F));          // up to 5kW depending on price 20c-100c/kWh
  sellToGrid = sellGridRate * micros2hrs * t_scan;                 // power * time = energy in Wh
  if ( sellGridRate > 2000.0F ) return true;    
  sellGridRate = 0.0F;  
  sellToGrid = 0.0F;                      
  return false;
}

bool decideToBuy(float battLevel, float battCapacity) {
  // buy based on price, battery level vs hours of day left to charge
  
  uint8_t hrsFromDawn = hour() - firstPVkW;
  uint8_t hrsInDay = 24 - 2*firstPVkW;
  if ( hour() >= firstPVkW && hour() <= (24 - firstPVkW) ) {            // if daytime
  // buy if battery will not reach say 16kWh by dusk
    if ( battCapacity/16.0F < hrsFromDawn/hrsInDay ) {                
      buyGridRate = max(0.0F,min(5.0F,5.0F-amberPrice))*1000;             // up to 5kW rate depending on price
    }
  // allow for panic buying after 2pm 
    else if ( battLevel < 8.0F && hour() >= 14 ) {                        
      buyGridRate = max(0.0F,min(5.0F,20.0F-amberPrice))*1000;            // up to 5kW rate depending on price
    } 
  // to do: interface to grid input to battery charger
    buyFromGrid = buyGridRate * micros2hrs * t_scan;                      // power * time = energy in Wh
    if (buyFromGrid > 0.0F) return true;
  }   
  buyGridRate = 0.0F;                  
  // to do: interface to grid input to battery charger
  return false;       
}
