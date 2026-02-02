#include <Arduino.h>

const int OIL_SWITCH_PIN = 18;   // Active LOW
const int LED_PIN        = 2;    // Built-in LED
const int OUT_PIN        = 19;   // Second output

// Phase durations (ms)
const unsigned long PHASE1_TIME = 10000;   // very slow
const unsigned long PHASE2_TIME = 20000;  // slow
// after this → fast (no limit)

// Blink timings (10% ON / 90% OFF)
const unsigned long VSLOW_ON  = 100;
const unsigned long VSLOW_OFF = 1900;

const unsigned long SLOW_ON   = 100;
const unsigned long SLOW_OFF  = 900;

const unsigned long FAST_ON   = 30;
const unsigned long FAST_OFF  = 270;

unsigned long faultStartTime = 0;
unsigned long lastToggleTime = 0;

bool outputState = false;
bool faultActive = false;

void setOutputs(bool state) {
    digitalWrite(LED_PIN, state);
    digitalWrite(OUT_PIN, state);
}

void setup() {
    pinMode(OIL_SWITCH_PIN, INPUT_PULLUP);
    pinMode(LED_PIN, OUTPUT);
    pinMode(OUT_PIN, OUTPUT);

    setOutputs(LOW);
}

void loop() {
    unsigned long now = millis();
    int oilState = digitalRead(OIL_SWITCH_PIN);

    // -------- Pressure OK --------
    if (oilState == HIGH) {
        faultActive = false;
        setOutputs(LOW);
        return;
    }

    // -------- Pressure NOT OK --------
    if (!faultActive) {
        // New fault → restart blinking from very slow
        faultActive = true;
        faultStartTime = now;
        lastToggleTime = now;
        outputState = false;
        setOutputs(LOW);
    }

    unsigned long elapsed = now - faultStartTime;

    unsigned long onTime, offTime;

    if (elapsed < PHASE1_TIME) {
        // Very slow
        onTime  = VSLOW_ON;
        offTime = VSLOW_OFF;
    }
    else if (elapsed < PHASE2_TIME) {
        // Slow
        onTime  = SLOW_ON;
        offTime = SLOW_OFF;
    }
    else {
        // Fast
        onTime  = FAST_ON;
        offTime = FAST_OFF;
    }

    unsigned long interval = outputState ? onTime : offTime;

    if (now - lastToggleTime >= interval) {
        lastToggleTime = now;
        outputState = !outputState;
        setOutputs(outputState);
    }
}
