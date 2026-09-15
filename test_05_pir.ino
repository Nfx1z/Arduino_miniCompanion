// ============================================================================
//  MODULE TEST 5: PIR Motion Sensor
//  Verifies: raw digitalRead and reproduces the 10s inactivity -> sleep
//  countdown logic so you can validate PIR settle time / sensitivity pot
//  before relying on it in the full state machine.
//  Wiring: PIR OUT -> GPIO10
// ============================================================================

#define PIN_PIR 4
#define PIR_SLEEP_TIMEOUT 10000UL  // 10s inactive -> sleep

bool          pirDetected      = false;
unsigned long pirInactiveStart = 0;
unsigned long lastPrint        = 0;

void setup() {
    Serial.begin(115200);
    delay(300);
    Serial.println(F("\n--- PIR Module Test ---"));

    pinMode(PIN_PIR, INPUT);

    Serial.println(F("[OK] Pin configured"));
    Serial.println(F("NOTE: most PIR modules need 30-60s warm-up after power-on"));
    Serial.println(F("      before readings stabilize -- ignore erratic output at boot."));
    Serial.println(F("Move in front of the sensor, then hold still to watch the"));
    Serial.println(F("10s countdown to simulated SLEEP."));
}

void loop() {
    unsigned long now = millis();
    pirDetected = (digitalRead(PIN_PIR) == HIGH);

    if (pirDetected) {
        pirInactiveStart = 0;
    } else if (pirInactiveStart == 0) {
        pirInactiveStart = now;
    }

    if (now - lastPrint >= 300) {
        lastPrint = now;
        Serial.print(F("motion="));
        Serial.print(pirDetected ? F("YES") : F("no "));

        if (!pirDetected && pirInactiveStart != 0) {
            unsigned long elapsed = now - pirInactiveStart;
            long remaining = (long)PIR_SLEEP_TIMEOUT - (long)elapsed;
            if (remaining <= 0) {
                Serial.println(F("  -> SLEEP would trigger now"));
            } else {
                Serial.print(F("  -> sleep in "));
                Serial.print(remaining / 1000.0, 1);
                Serial.println(F("s"));
            }
        } else {
            Serial.println();
        }
    }
}
