#include <Arduino.h>
#include <stdint.h>
#include <math.h>

/**
 * Represents the boundary values of a PWM pin on an Arduino board.
 */
enum PwmBoundaryValues : uint8_t {
  kLow = 0,
  kHigh = UINT8_MAX
};


/**
 * Represents the boundary values of a digital pin on an Arduino board.
 */
enum DigitalBoundaryValues : uint8_t {
  kLow = LOW,
  kHigh = HIGH
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
      val, DigitalBoundaryValues::kLow, DigitalBoundaryValues::kHigh
    );
    writeValue();
  }

  void setValue(double val) {
    double constrVal = constrain(val, 0, 1);
    currentValue = (uint8_t)lround(
      constrVal * DigitalBoundaryValues::kHigh + DigitalBoundaryValues::kLow
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
      case DigitalBoundaryValues::kHigh:
        digitalWrite(pinNumber, DigitalBoundaryValues::kHigh - currentValue);
        break;
      case DigitalBoundaryValues::kLow:
        digitalWrite(pinNumber, currentValue - DigitalBoundaryValues::kLow);
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
      val, PwmBoundaryValues::kLow, PwmBoundaryValues::kHigh
    );
    writeValue();
  }

  void setValue(double val) {
    double constrVal = constrain(val, 0, 1);
    currentValue = (uint8_t)lround(
      constrVal * PwmBoundaryValues::kHigh + PwmBoundaryValues::kLow
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
      case PwmBoundaryValues::kHigh:
        analogWrite(pinNumber, PwmBoundaryValues::kHigh - currentValue);
        break;
      case PwmBoundaryValues::kLow:
        analogWrite(pinNumber, currentValue - PwmBoundaryValues::kLow);
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
    led2.setValue((uint8_t)(PwmBoundaryValues::kHigh - val));
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
