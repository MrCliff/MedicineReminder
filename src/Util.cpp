#include <stdint.h>
#include <math.h>

double dMap(double value, double fromMin, double fromMax, double toMin,
    double toMax) {
  return ((value - fromMin) / (fromMax - fromMin)) * (toMax - toMin) + toMin;
}
