#include <arduino.h>

extern float wattsBarrel[8][8];
extern uint8_t wattsIndex;
float sum;

float avgWatts(float w, uint8_t c, uint8_t d) {
    if ( d > 8 || c > 7 ) return 0.0F;
    wattsBarrel[c][wattsIndex] = w;
    wattsIndex = (wattsIndex+1)%d;
    sum = 0.0F;
    for (int i = 0; i<d; i++) sum += wattsBarrel[c][i];
    sum /= (float)d;
    return sum;
}