#include <extern.h>
// Energy reset at midnight in minProc
void dailyEnergy() {
  t_scan = millis() - t_lastData;
  t_lastData = millis();
  for ( int i = 1;i<NUM_CHANNELS+1;i++ ) {
    if (Wrms[i] > 8.0) Energy[i] += Wrms[i]*(float)t_scan/3.6e9;  // kWh
  }
}