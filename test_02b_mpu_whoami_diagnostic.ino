// ============================================================================
//  DIAGNOSTIC: Raw WHO_AM_I read for MPU-family chip at 0x68
//  Bypasses the Adafruit_MPU6050 library's internal sanity check so we can
//  see exactly what chip ID the board is reporting.
// ============================================================================

#include <Wire.h>

#define PIN_I2C_SDA 6
#define PIN_I2C_SCL 7
#define MPU_ADDR    0x68
#define REG_WHO_AM_I 0x75

void setup() {
    Serial.begin(115200);
    delay(300);
    Serial.println(F("\n--- MPU WHO_AM_I Diagnostic ---"));

    Wire.begin(PIN_I2C_SDA, PIN_I2C_SCL);
    Wire.setClock(100000);  // slow, conservative clock for this test

    Wire.beginTransmission(MPU_ADDR);
    Wire.write(REG_WHO_AM_I);
    uint8_t err = Wire.endTransmission(false);
    Wire.requestFrom(MPU_ADDR, (uint8_t)1);

    if (Wire.available()) {
        uint8_t whoami = Wire.read();
        Serial.print(F("endTransmission error code: "));
        Serial.println(err);
        Serial.print(F("WHO_AM_I register value: 0x"));
        Serial.println(whoami, HEX);
        Serial.println();

        switch (whoami) {
            case 0x68:
                Serial.println(F("-> Genuine MPU6050. Adafruit_MPU6050 library should work."));
                Serial.println(F("   If begin() still fails, suspect power/noise, not chip ID."));
                break;
            case 0x70:
                Serial.println(F("-> This is an MPU6500, not an MPU6050."));
                Serial.println(F("   Adafruit_MPU6050 will reject it. Use Adafruit_MPU6050 anyway"));
                Serial.println(F("   only if the datasheet confirms register compat, OR switch to"));
                Serial.println(F("   Adafruit_MPU6500 / bolderflight MPU9250 library instead."));
                break;
            case 0x71:
                Serial.println(F("-> This is an MPU9250 (has magnetometer). Very common on"));
                Serial.println(F("   'GY-91'/'GY-9250' boards mislabeled as GY-521/MPU6050."));
                Serial.println(F("   Use an MPU9250-specific library (e.g. bolderflight/MPU9250)."));
                break;
            case 0x73:
                Serial.println(F("-> This is an MPU9255. Same family as 9250, needs its own library."));
                break;
            case 0x00:
            case 0xFF:
                Serial.println(F("-> No real response (0x00/0xFF often means bus fault, not a chip)."));
                Serial.println(F("   Check SDA/SCL wiring, pull-ups, and power to the board."));
                break;
            default:
                Serial.println(F("-> Unrecognized ID. Check module silkscreen/datasheet."));
                break;
        }
    } else {
        Serial.println(F("[FAIL] No data returned -- device did not respond to read request."));
        Serial.println(F("Check wiring, power, and that nothing else on the bus is address-conflicting."));
    }
}

void loop() {
    // one-shot diagnostic; nothing to repeat
}
