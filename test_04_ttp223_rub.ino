// ============================================================================
//  MODULE TEST 4: TTP223 Touch Sensors (rub tracking)
//  Verifies: raw digitalRead on both touch pads, and reproduces your
//  rubAccumulated logic so you can confirm the WAKE (6s) / HAPPY (3s)
//  thresholds feel right physically before wiring into the full state machine.
//  Wiring: TTP223 #1 -> GPIO4, TTP223 #2 -> GPIO5
// ============================================================================

#define PIN_TTP223_1 4
#define PIN_TTP223_2 5

#define RUB_RESET_TIMEOUT    500UL    // ms of no touch before accumulator resets
#define RUB_HAPPY_THRESHOLD  3000UL   // 3s rub -> HAPPY (from PLAIN)
#define RUB_WAKE_THRESHOLD   6000UL   // 6s rub -> WAKE (from SLEEP)

bool          isRubbing      = false;
unsigned long rubStartTime   = 0;
unsigned long rubAccumulated = 0;
unsigned long lastRubTime    = 0;
unsigned long lastPrint      = 0;

void updateRubTracking(unsigned long now) {
    bool touching = (digitalRead(PIN_TTP223_1) == HIGH) ||
                    (digitalRead(PIN_TTP223_2) == HIGH);

    if (touching) {
        if (!isRubbing) {
            isRubbing = true;
            if (rubAccumulated == 0) {
                rubStartTime = now;
            }
        }
        lastRubTime    = now;
        rubAccumulated = now - rubStartTime;
    } else {
        isRubbing = false;
        if (now - lastRubTime > RUB_RESET_TIMEOUT) {
            rubAccumulated = 0;
            rubStartTime   = 0;
        }
    }
}

void setup() {
    Serial.begin(115200);
    delay(300);
    Serial.println(F("\n--- TTP223 Touch/Rub Module Test ---"));

    pinMode(PIN_TTP223_1, INPUT);
    pinMode(PIN_TTP223_2, INPUT);

    Serial.println(F("[OK] Pins configured"));
    Serial.println(F("Touch and hold either pad continuously to accumulate rub time."));
    Serial.println(F("Lifting off for >500ms resets the accumulator (matches production logic)."));
    Serial.print(F("HAPPY threshold: ")); Serial.print(RUB_HAPPY_THRESHOLD);
    Serial.print(F("ms | WAKE threshold: ")); Serial.print(RUB_WAKE_THRESHOLD);
    Serial.println(F("ms"));
}

void loop() {
    unsigned long now = millis();
    updateRubTracking(now);

    if (now - lastPrint >= 150) {
        lastPrint = now;
        bool pad1 = digitalRead(PIN_TTP223_1) == HIGH;
        bool pad2 = digitalRead(PIN_TTP223_2) == HIGH;

        Serial.print(F("pad1=")); Serial.print(pad1);
        Serial.print(F(" pad2=")); Serial.print(pad2);
        Serial.print(F(" | rubAccumulated=")); Serial.print(rubAccumulated);
        Serial.print(F("ms"));

        if (rubAccumulated >= RUB_WAKE_THRESHOLD) {
            Serial.println(F("  -> WAKE threshold reached"));
        } else if (rubAccumulated >= RUB_HAPPY_THRESHOLD) {
            Serial.println(F("  -> HAPPY threshold reached"));
        } else {
            Serial.println();
        }
    }
}
