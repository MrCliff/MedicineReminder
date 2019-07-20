#ifndef UTIL_H
#define UTIL_H

/**
 * Maps the given value from one range to other. This function uses doubles
 * instead of integer values that the Arduino library's map uses.
 */
double dMap(double value, double fromMin, double fromMax, double toMin,
    double toMax);

#endif
