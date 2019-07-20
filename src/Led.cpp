#ifndef LED
#define LED

#include <Arduino.h>
#include <stdint.h>
#include <math.h>
#include "Util.h"

/**
 * Represents the boundary values of a PWM pin on an Arduino board.
 */
enum PwmBoundaryValues : uint8_t {
  kLowPwm = 0,
  kHighPwm = UINT8_MAX
};


/**
 * Represents the boundary values of a digital pin on an Arduino board.
 */
enum DigitalBoundaryValues : uint8_t {
  kLowDig = LOW,
  kHighDig = HIGH
};


/**
 * Represents one LED connected to a digital pin.
 */
class Led {
  const DigitalBoundaryValues offValue;
  const uint8_t pinNumber;

  DigitalBoundaryValues currentValue;

public:
  Led(uint8_t pinNumber, DigitalBoundaryValues offValue) :
    pinNumber(pinNumber), offValue(offValue) {
  }

  void begin() {
    pinMode(pinNumber, OUTPUT);
    digitalWrite(pinNumber, offValue);
  }

  /**
   * Sets the LED value to the given byte value (value between 0 and 255).
   */
  void setByteValue(uint8_t val) {
    currentValue = val >= (kHighDig - kLowDig) / 2.0 ? kHighDig : kLowDig;
    writeValue();
  }

  /**
   * Sets the LED value to the given percentage (value between 0 and 1).
   */
  void setValue(double val) {
    currentValue = val >= 0.5 ? kHighDig : kLowDig;
    writeValue();
  }

  /**
   * Returns the currently assigned value.
   */
  DigitalBoundaryValues getValue() const {
    return currentValue;
  }

  /**
   * Writes the current value to the output.
   */
  void writeValue() const {
    DigitalBoundaryValues onValue = offValue == kHighDig ? kLowDig : kHighDig;
    digitalWrite(pinNumber,
      map(currentValue, kLowDig, kHighDig, offValue, onValue)
    );
  }
};


/**
 * Represents one LED connected to a PWM pin.
 */
class PwmLed {
  const PwmBoundaryValues offValue;
  const uint8_t pinNumber;
  const double gamma;

  uint8_t currentValue;

public:
  PwmLed(uint8_t pinNumber, PwmBoundaryValues offValue, double gamma) :
    pinNumber(pinNumber), offValue(offValue), gamma(gamma) {
  }

  void begin() {
    pinMode(pinNumber, OUTPUT);
    analogWrite(pinNumber, offValue);
  }

  /**
   * Sets the LED value to the given byte value (value between 0 and 255).
   */
  void setByteValue(uint8_t val) {
    currentValue = constrain(
      val, kLowPwm, kHighPwm
    );
    writeValue();
  }

  /**
   * Sets the LED value to the given percentage (value between 0 and 1).
   */
  void setValue(double val) {
    double constrVal = constrain(val, 0, 1);
    currentValue = (uint8_t)lround(
      constrVal * kHighPwm + kLowPwm
    );
    writeValue();
  }

  /**
   * Returns the currently assigned value.
   */
  uint8_t getValue() const {
    return currentValue;
  }

  /**
   * Writes the current value to the output (with Gamma correction).
   */
  void writeValue() const {
    PwmBoundaryValues onValue = offValue == kHighPwm ? kLowPwm : kHighPwm;
    analogWriteWithGammaCorrection(
      map(currentValue, kLowPwm, kHighPwm, offValue, onValue)
    );
  }

private:
  /**
   * Gamma corrects and analogWrites the given analogValue.
   */
  void analogWriteWithGammaCorrection(uint8_t analogValue) {
    double percentage = dMap(analogValue, kLowPwm, kHighPwm, 0.0, 1.0);
    analogWrite(pinNumber, (uint8_t)constrain(lround(dMap(
      pow(percentage, gamma),
      0.0, 1.0,
      kLowPwm, kHighPwm
    )), kLowPwm, kHighPwm));
  }
};


/**
 * Represents two LEDs connected to PWM pins.
 */
class DualPwmLed {
  PwmLed led1;
  PwmLed led2;

public:
  DualPwmLed(uint8_t led1Pin, uint8_t led2Pin, PwmBoundaryValues offValue,
    double gamma) :
    DualPwmLed(led1Pin, led2Pin, offValue, offValue, gamma) {
  }

  DualPwmLed(uint8_t led1Pin, uint8_t led2Pin, PwmBoundaryValues offValue1,
    PwmBoundaryValues offValue2, double gamma) :
    led1(led1Pin, offValue1, gamma), led2(led2Pin, offValue2, gamma) {
  }

  void begin() {
    led1.begin();
    led2.begin();
  }

  /**
   * Sets the LED value to the given byte value (value between 0 and 255).
   */
  void setBipolarByteValue(uint8_t val) {
    led1.setByteValue(val);
    led2.setByteValue((uint8_t)(kHighPwm - val));
  }

  /**
   * Sets the LED value to the given percentage (value between 0 and 1).
   */
  void setBipolarValue(double val) {
    double constrVal = constrain(val, 0, 1);
    led1.setValue(constrVal);
    led2.setValue(1.0 - constrVal);
  }

  /**
   * Returns the value currently assigned to LED 1.
   */
  uint8_t getLed1Value() const {
    return led1.getValue();
  }

  /**
   * Returns the value currently assigned to LED 2.
   */
  uint8_t getLed2Value() const {
    return led2.getValue();
  }

  void writeValue() const {
    led1.writeValue();
    led2.writeValue();
  }
};


/**
 * Holds a value that can be changed over time.
 */
class ValueChanger {
  const double fullChangeTimeSec;
  uint32_t targetValue;

public:
  ValueChanger(double fullChangeTimeSec, uint32_t targetValue) :
    fullChangeTimeSec(fullChangeTimeSec), targetValue(targetValue) {
  }

  void setTargetValue(const uint32_t targetValue) {
    this->targetValue = targetValue;
  }

  /**
   * Returns the value at the given time instant.
   */
  double getValueAfter(double timeSec) const {
    return timeSec / fullChangeTimeSec * targetValue;
  }
};

#endif
