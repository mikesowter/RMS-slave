#include <arduino.h>

extern float wattsBarrel[8][25];
extern uint8_t wattsIndex;
float sum;

float smoothWatts(float w, uint8_t cct, uint8_t d) {
    wattsBarrel[cct][wattsIndex] = w;
    wattsIndex = (wattsIndex+1)%d;
    sum = 0.0F;
    for (int i = 0; i<d; i++) sum += wattsBarrel[cct][i];
    sum /= (float)d;
    return sum;
}