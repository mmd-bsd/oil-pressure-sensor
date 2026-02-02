#include <Arduino.h>

const int OIL_SWITCH_PIN = 18;   // Active LOW
const int LED_PIN        = 2;    // Built-in LED
const int OUT_PIN        = 19;   // Second output

// Time thresholds
const unsigned long SLOW_BLINK_TIME = 10000; // 0–10s
const unsigned long FAST_BLINK_TIME = 15000; // 10–15s

// Blink timing (ms)
// Slow blink: total 1000ms → 100ms ON / 900ms OFF
const unsigned long SLOW_ON_TIME  = 100;
const unsigned long SLOW_OFF_TIME = 900;

// Fast blink: total 300ms → 30ms ON / 270ms OFF
const unsigned long FAST_ON_TIME  = 30;
const unsigned long FAST_OFF_TIME = 270;

unsigned long startTime;
unsigned long lastToggleTime = 0;
bool outputState = false;

bool oilWasOkOnce = false;

void setOutputs(bool state) {
    digitalWrite(LED_PIN, state);
    digitalWrite(OUT_PIN, state);
}

void setup() {
    pinMode(OIL_SWITCH_PIN, INPUT_PULLUP);
    pinMode(LED_PIN, OUTPUT);
    pinMode(OUT_PIN, OUTPUT);

    setOutputs(LOW);
    startTime = millis();
}

void loop() {
    unsigned long now = millis();
    unsigned long elapsed = now - startTime;

    int oilState = digitalRead(OIL_SWITCH_PIN);

    // --- Oil pressure OK ---
    if (oilState == HIGH) {
        oilWasOkOnce = true;
        setOutputs(LOW);
        return;
    }

    // --- Pressure dropped after being OK ---
    if (oilWasOkOnce) {
        setOutputs(HIGH);   // solid ON
        return;
    }

    // --- Startup blinking phase ---
    unsigned long onTime, offTime;

    if (elapsed < SLOW_BLINK_TIME) {
        onTime  = SLOW_ON_TIME;
        offTime = SLOW_OFF_TIME;
    }
    else if (elapsed < FAST_BLINK_TIME) {
        onTime  = FAST_ON_TIME;
        offTime = FAST_OFF_TIME;
    }
    else {
        // After 15s and still not OK
        setOutputs(HIGH);
        return;
    }

    // --- Non-blocking blink with duty cycle ---
    unsigned long interval = outputState ? onTime : offTime;

    if (now - lastToggleTime >= interval) {
        lastToggleTime = now;
        outputState = !outputState;
        setOutputs(outputState);
    }
}
