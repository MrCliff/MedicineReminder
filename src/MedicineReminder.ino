#include <Sodaq_DS3231.h>
#include <stdint.h>
#include "Led.cpp"

const int kPinLedR = 9;
const int kPinLedG = 10;
const int kPinLedB = 11;
const int kPinBtn = 8;

DualPwmLed *dualLedRG;
PwmLed *ledB;

uint8_t ledRGValue = 0;
uint8_t switchSpeed = 1;

void setup() {
    // put your setup code here, to run once:
    dualLedRG = new DualPwmLed(kPinLedG, kPinLedR, PwmBoundaryValues::kHigh);
    ledB = new PwmLed(kPinLedB, PwmBoundaryValues::kHigh);

    pinMode(kPinBtn, INPUT);
}

void loop() {
    // put your main code here, to run repeatedly:
    ledRGValue += switchSpeed;

    dualLedRG->setTwoPoleValue(ledRGValue);
    dualLedRG->writePins();

    delay(10);
}
