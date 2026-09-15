// ============================================================================
//  MODULE TEST 3: SHT30D Temperature/Humidity
//  Verifies: I2C init, periodic read, HOT/COLD threshold evaluation
//  Wiring: SDA -> GPIO6, SCL -> GPIO7, addr 0x44
// ============================================================================

#include <Wire.h>
#include <Adafruit_SHT31.h>

#define PIN_I2C_SDA        6
#define PIN_I2C_SCL        7
#define TEMP_READ_INTERVAL 2000UL   // faster than production (5000ms) for quick testing
#define TEMP_HOT_THRESHOLD  33.0f
#define TEMP_COLD_THRESHOLD 20.0f

Adafruit_SHT31 sht30 = Adafruit_SHT31();
unsigned long lastRead = 0;

void setup() {
    Serial.begin(115200);
    delay(300);
    Serial.println(F("\n--- SHT30D Module Test ---"));

    Wire.begin(PIN_I2C_SDA, PIN_I2C_SCL);
    Wire.setClock(400000);

    if (!sht30.begin(0x44)) {
        Serial.println(F("[FAIL] SHT30D not found at 0x44. Check wiring / try 0x45."));
        while (true) { yield(); }
    }
    Serial.println(F("[OK] SHT30D initialized"));
    Serial.print(F("HOT threshold: "));  Serial.print(TEMP_HOT_THRESHOLD);
    Serial.print(F(" C | COLD threshold: ")); Serial.println(TEMP_COLD_THRESHOLD);
}

void loop() {
    unsigned long now = millis();
    if (now - lastRead >= TEMP_READ_INTERVAL) {
        lastRead = now;

        float t = sht30.readTemperature();
        float h = sht30.readHumidity();

        if (isnan(t)) {
            Serial.println(F("[WARN] Temperature read returned NaN -- sensor comm issue"));
            return;
        }

        bool hot  = (t > TEMP_HOT_THRESHOLD);
        bool cold = (t < TEMP_COLD_THRESHOLD);

        Serial.print(F("Temp: "));
        Serial.print(t, 2);
        Serial.print(F(" C | Humidity: "));
        Serial.print(isnan(h) ? -1.0f : h, 1);
        Serial.print(F(" % | "));
        if (hot)       Serial.println(F("-> HOT flag would trigger"));
        else if (cold) Serial.println(F("-> COLD/FREEZE flag would trigger"));
        else           Serial.println(F("-> within normal range"));
    }
}
