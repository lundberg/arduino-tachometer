#include "Tachometer.h"


Tachometer::Tachometer(uint8_t pin, uint8_t ppr) {
    this->pin = pin;
    this->ppr = ppr;
    this->reset();
}


void Tachometer::begin() {
    pinMode(this->pin, INPUT);
}


void Tachometer::begin(void (*interrupt)()) {
    begin();
    this->interrupt = interrupt;
    this->enableInterrupt();
}


void Tachometer::trigger() {
    unsigned long now = micros();

    if (this->pulseTimestamp > 0) {
        unsigned long width = now - this->pulseTimestamp;

        // Ignore short pulse widths / bounce to prevent noise
        if (width < TACH_MIN_PULSE_WIDTH) {
            Serial.print("-- ignoring pulse -- ");
            Serial.print(width);
            Serial.print(" vs ");
            Serial.println(this->pulseWidth);
            return;
        }

        // Record pulse width
        this->pulse(width);
    }

    this->pulseTimestamp = now;
}


bool Tachometer::update() {
    if (!this->interrupt) {
        this->inspectPin();
    }

    bool updated = this->updatePulseWidth();

    if (!updated) {
        unsigned long idleTime = micros() - this->pulseTimestamp;

        if (idleTime > TACH_MAX_PULSE_WIDTH) {
            if (this->pulseWidth) {
                // Pulse zeros til median becomes 0
                this->pulse(0);
                updated = this->updatePulseWidth();
            } else {
                // Reset everything once pulse width is 0
                this->reset();
            }
        }
    }

    return updated;
}


int Tachometer::getHZ() {
    return round(SECOND_MICROS / pulseWidth);
}


int Tachometer::getRPM() {
    return getRPM(false);
}


int Tachometer::getRPM(bool smooth) {
    float revolutionTime = pulseWidth * ppr;
    float rpm = round(MINUTE_MICROS / revolutionTime);

    if (smooth && rpm) {
        int precision;

        if (rpm < 900)       precision = 1.0;
        else if (rpm < 1000) precision = 5.0;
        else if (rpm < 2000) precision = 5.0;  //10.0;
        else if (rpm < 3500) precision = 10.0;  //50.0;
        else                 precision = 50.0;  //100.0;

        rpm = round(rpm / precision) * precision;
    }

    return rpm;
}
