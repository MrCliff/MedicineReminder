#include <Arduino.h>
#include <stdint.h>


enum PwmBoundaryValues {
    kLow = 0,
    kHigh = UINT8_MAX
};

/**
 * Represents one LED connected to a PWM pin.
 */
class PwmLed {
public:
    PwmLed(int pinNumber, PwmBoundaryValues offValue) : pinNumber(pinNumber), offValue(offValue) {
        pinMode(pinNumber, OUTPUT);
        analogWrite(pinNumber, offValue);
    }

    void setValue(uint8_t val) {
        currentValue = val;
    }

    uint8_t getValue() const {
        return currentValue;
    }

    /**
     * Writes the current value to the output.
     */
    void writeValue() const {
        switch (offValue) {
            case kHigh:
                analogWrite(pinNumber, kHigh - currentValue);
            case kLow:
                analogWrite(pinNumber, currentValue - kLow);
        }
    }

private:
    const PwmBoundaryValues offValue;
    const int pinNumber;

    uint8_t currentValue;
};


/**
 * Represents two LEDs connected to PWM pins.
 */
class DualPwmLed {
public:
    DualPwmLed(int led1Pin, int led2Pin, PwmBoundaryValues offValue) :
            DualPwmLed(led1Pin, led2Pin, offValue, offValue) {
    }

    DualPwmLed(int led1Pin, int led2Pin, PwmBoundaryValues offValue1, PwmBoundaryValues offValue2) :
            led1(led1Pin, offValue1), led2(led2Pin, offValue2) {
    }

    void setTwoPoleValue(uint8_t val) {
        led1.setValue(val);
        led2.setValue(kHigh - val);
    }

    uint8_t getLed1Value() const {
        return led1.getValue();
    }

    uint8_t getLed2Value() const {
        return led2.getValue();
    }

    void writePins() const {
        led1.writeValue();
        led2.writeValue();
    }

private:
    PwmLed led1;
    PwmLed led2;
};
