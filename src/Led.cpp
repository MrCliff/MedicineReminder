#include <Arduino.h>
#include <stdint.h>
#include <math.h>

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
public:
  Led(uint8_t pinNumber, DigitalBoundaryValues offValue) :
    pinNumber(pinNumber), offValue(offValue) {
  }

  void begin() {
    pinMode(pinNumber, OUTPUT);
    digitalWrite(pinNumber, offValue);
  }

  void setValue(uint8_t val) {
    currentValue = constrain(
      val, kLowDig, kHighDig
    );
    writeValue();
  }

  void setValue(double val) {
    double constrVal = constrain(val, 0, 1);
    currentValue = (uint8_t)lround(
      constrVal * kHighDig + kLowDig
    );
    writeValue();
  }

  uint8_t getValue() const {
    return currentValue;
  }

  /**
   * Writes the current value to the output.
   */
  void writeValue() const {
    switch (offValue) {
      case kHighDig:
        digitalWrite(pinNumber, kHighDig - currentValue);
        break;
      case kLowDig:
        digitalWrite(pinNumber, currentValue - kLowDig);
        break;
    }
  }

private:
  const DigitalBoundaryValues offValue;
  const uint8_t pinNumber;

  uint8_t currentValue;
};


/**
 * Represents one LED connected to a PWM pin.
 */
class PwmLed {
public:
  PwmLed(uint8_t pinNumber, PwmBoundaryValues offValue) :
    pinNumber(pinNumber), offValue(offValue) {
  }

  void begin() {
    pinMode(pinNumber, OUTPUT);
    analogWrite(pinNumber, offValue);
  }

  void setValue(uint8_t val) {
    currentValue = constrain(
      val, kLowPwm, kHighPwm
    );
    writeValue();
  }

  void setValue(double val) {
    double constrVal = constrain(val, 0, 1);
    currentValue = (uint8_t)lround(
      constrVal * kHighPwm + kLowPwm
    );
    writeValue();
  }

  uint8_t getValue() const {
    return currentValue;
  }

  /**
   * Writes the current value to the output.
   */
  void writeValue() const {
    switch (offValue) {
      case kHighPwm:
        analogWrite(pinNumber, kHighPwm - currentValue);
        break;
      case kLowPwm:
        analogWrite(pinNumber, currentValue - kLowPwm);
        break;
    }
  }

private:
  const PwmBoundaryValues offValue;
  const uint8_t pinNumber;

  uint8_t currentValue;
};


/**
 * Represents two LEDs connected to PWM pins.
 */
class DualPwmLed {
public:
  DualPwmLed(uint8_t led1Pin, uint8_t led2Pin, PwmBoundaryValues offValue) :
    DualPwmLed(led1Pin, led2Pin, offValue, offValue) {
  }

  DualPwmLed(uint8_t led1Pin, uint8_t led2Pin, PwmBoundaryValues offValue1,
    PwmBoundaryValues offValue2) :
    led1(led1Pin, offValue1), led2(led2Pin, offValue2) {
  }

  void begin() {
    led1.begin();
    led2.begin();
  }

  void setTwoPoleValue(uint8_t val) {
    led1.setValue(val);
    led2.setValue((uint8_t)(kHighPwm - val));
  }

  void setTwoPoleValue(double val) {
    double constrVal = constrain(val, 0, 1);
    led1.setValue(constrVal);
    led2.setValue(1.0 - constrVal);
  }

  uint8_t getLed1Value() const {
    return led1.getValue();
  }

  uint8_t getLed2Value() const {
    return led2.getValue();
  }

  void writeValue() const {
    led1.writeValue();
    led2.writeValue();
  }

private:
  PwmLed led1;
  PwmLed led2;
};


/**
 * Holds a value that can be changed in an accelerating manner.
 */
class Accelerator {
public:
  Accelerator(double rate) : rate(rate) {
  }

  void accelerate() {
    acceleration += rate;
    value += acceleration;
  }

  void reset() {
    acceleration = 0.0;
    value = 0.0;
  }

  double getValue() const {
    return value;
  }

private:
  const double rate;
  double acceleration;
  double value;
};
