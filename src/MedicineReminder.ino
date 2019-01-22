#include <Sodaq_DS3231.h>
#include <stdint.h>
#include "Led.cpp"

const uint8_t kPinPWMLedR = 9;
const uint8_t kPinPWMLedG = 10;
const uint8_t kPinLedB = 11;
const uint8_t kPinInterruptBtn = 2;
const uint8_t kPinRecordBtn = 8;
const uint8_t kPinFastForwardBtn = 7;

// Arduino handles integers as 16 bit values, so 12 hours (43200s) would
// overflow and become a negative value, if there wasn't a type cast.
const uint32_t kShiftTimeSec = (uint32_t)60 * 60 * 12;

DualPwmLed dualLedRG = DualPwmLed(kPinPWMLedG, kPinPWMLedR, PwmBoundaryValues::kHigh);
PwmLed ledB = Led(kPinLedB, DigitalBoundaryValues::kHigh);

uint32_t lastResetTs = 0;

volatile bool resetTsFlag = false;

Accelerator ffAccelerator(2.0);

void setup() {
  dualLedRG.begin();
  ledB.begin();

  rtc.begin();
  lastResetTs = rtc.now().getEpoch();

  Serial.begin(115200);

  // Buttons
  pinMode(kPinRecordBtn, INPUT);
  pinMode(kPinFastForwardBtn, INPUT);
  attachInterrupt(digitalPinToInterrupt(kPinInterruptBtn), resetTs, RISING);
}

void loop() {
  DateTime now = rtc.now(); // get the current date-time
  uint32_t ts = now.getEpoch();

  // Reset
  if (resetTsFlag) {
    lastResetTs = ts;

    resetTsFlag = false;
  }

  // Fast forward
  if (digitalRead(kPinFastForwardBtn)) {
    ffAccelerator.accelerate();
    lastResetTs = (uint32_t)constrain((double)lastResetTs - ffAccelerator.getValue(), (double)0, (double)ts);
  }
  else {
    ffAccelerator.reset();
  }

  lastResetTs = (uint32_t)constrain(lastResetTs, (uint32_t)0, ts);
  double ledRGValue = 1.0 - ((double)(ts - lastResetTs) / kShiftTimeSec);
  dualLedRG.setTwoPoleValue(ledRGValue);
  Serial.print("LedValue: ");
  Serial.print(ledRGValue);
  Serial.print("; Epoch: ");
  Serial.print(ts);
  Serial.print("; lastResetEpoch: ");
  Serial.print(lastResetTs);
  Serial.print("; kShiftTimeSec: ");
  Serial.print(kShiftTimeSec);
  Serial.print("; (uint32_t)60 * 60 * 12: ");
  Serial.print((uint32_t)60 * 60 * 12);
  Serial.print("; ts - lastResetTs: ");
  Serial.print(ts - lastResetTs);
  Serial.print("; ts - lastResetTs / kShiftTimeSec: ");
  Serial.print((double)(ts - lastResetTs) / kShiftTimeSec);
  Serial.print("; ffSpeed: ");
  Serial.println(ffAccelerator.getValue());

  delay(100);
}

void resetTs() {
  resetTsFlag = true;
}
