#ifndef __TACHOMETER__
#define __TACHOMETER__

#include <math.h>
#include <Arduino.h>
#include "Median.h"

#define SECOND_MICROS 1000000.0
#define MINUTE_MICROS 60.0 * SECOND_MICROS

#define TACH_MIN_PULSE_WIDTH SECOND_MICROS / 1000  // 1000 Hz 
#define TACH_MAX_PULSE_WIDTH SECOND_MICROS / 4     // 4 Hz 
#define TACH_PULSE_SAMPLE_COUNT 15


class Tachometer {

public:
    Tachometer(uint8_t pin, uint8_t ppr = 1);

    void begin();
    void begin(void (*interrupt)());

    void trigger();
    bool update();

    int getHZ();
    int getRPM();
    int getRPM(bool smooth);

private:
    uint8_t pin;
    uint8_t pinState = LOW;
    uint8_t ppr;  // Pulses Per Revolution

    unsigned long pulseWidth = 0;
    unsigned long pulseTimestamp = 0;
    volatile unsigned long pulseWidths[TACH_PULSE_SAMPLE_COUNT];
    volatile uint8_t pulseIndex = 0;
    volatile uint8_t pulseLag = 0;

    bool inspectPin() {
        bool state = digitalRead(this->pin);
        bool isRising = (state == HIGH && this->pinState == LOW);
        this->pinState = state;

        if (isRising) {
            trigger();
        }

        return isRising;
    }

    void (*interrupt)();

    void enableInterrupt() {
        attachInterrupt(digitalPinToInterrupt(this->pin), this->interrupt, RISING);
    }

    void disableInterrupt() {
        detachInterrupt(digitalPinToInterrupt(this->pin));
    }

    void reset() {
        this->pulseTimestamp = 0;

        this->pulseWidth = 0;
        this->pulseIndex = 0;
        this->pulseLag = 0;

        for (uint8_t i = 0; i < TACH_PULSE_SAMPLE_COUNT; i++) {
            this->pulseWidths[i] = 0;
        }
    }

    void pulse(long width) {
        pulseLag++;
        pulseWidths[pulseIndex++] = width;
        if (pulseIndex >= TACH_PULSE_SAMPLE_COUNT) {
            pulseIndex = 0;
        }
    }

    bool updatePulseWidth() {
        bool updated = false;

        if (pulseLag > 0) {
            long stats[TACH_PULSE_SAMPLE_COUNT];
            for (uint8_t i = 0; i < TACH_PULSE_SAMPLE_COUNT; i++) {
                stats[i] = pulseWidths[i];
            }

            // Calculate median pulse width
            unsigned long width = quick_select_median(stats, TACH_PULSE_SAMPLE_COUNT);

            // Update pulse width if changed
            if (width != this->pulseWidth) {
                this->pulseWidth = width;
                updated = true;
            }

            pulseLag = 0;
        }

        return updated;
    }

};

#endif
