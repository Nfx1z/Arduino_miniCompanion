// ============================================================================
//  MODULE TEST 2 (v2): MPU6500 via raw I2C registers
//  Confirmed chip: WHO_AM_I = 0x70 (MPU6500, not MPU6050)
//  Adafruit_MPU6050 library rejects this chip outright, but the accel/gyro
//  register map is compatible enough with MPU6050 that we don't need a new
//  library at all -- just talk to the registers directly.
//  Wiring: SDA -> GPIO6, SCL -> GPIO7, addr 0x68
// ============================================================================

#include <Wire.h>

#define PIN_I2C_SDA        6
#define PIN_I2C_SCL        7
#define MPU_ADDR           0x68
#define TILT_THRESHOLD_DEG 30.0f

// Registers (shared layout between MPU6050 / MPU6500)
#define REG_PWR_MGMT_1   0x6B
#define REG_ACCEL_CONFIG 0x1C
#define REG_ACCEL_XOUT_H 0x3B

// ACCEL_CONFIG = 0x00 -> +/-2g range -> 16384 LSB/g (same scale factor on both chips)
const float ACCEL_SCALE = 16384.0f;
const float GRAVITY     = 9.80665f;

unsigned long lastPrint = 0;

void writeReg(uint8_t reg, uint8_t val) {
    Wire.beginTransmission(MPU_ADDR);
    Wire.write(reg);
    Wire.write(val);
    Wire.endTransmission();
}

bool readAccelRaw(int16_t &ax, int16_t &ay, int16_t &az) {
    Wire.beginTransmission(MPU_ADDR);
    Wire.write(REG_ACCEL_XOUT_H);
    if (Wire.endTransmission(false) != 0) return false;

    if (Wire.requestFrom(MPU_ADDR, (uint8_t)6) != 6) return false;

    ax = (Wire.read() << 8) | Wire.read();
    ay = (Wire.read() << 8) | Wire.read();
    az = (Wire.read() << 8) | Wire.read();
    return true;
}

void setup() {
    Serial.begin(115200);
    delay(300);
    Serial.println(F("\n--- MPU6500 Module Test (raw register driver) ---"));

    Wire.begin(PIN_I2C_SDA, PIN_I2C_SCL);
    Wire.setClock(400000);

    // Wake the chip -- PWR_MGMT_1 defaults to sleep mode on power-up
    writeReg(REG_PWR_MGMT_1, 0x00);
    delay(50);

    // Set accel range to +/-2g (matches ACCEL_SCALE above)
    writeReg(REG_ACCEL_CONFIG, 0x00);
    delay(10);

    Serial.println(F("[OK] MPU6500 woken and configured for +/-2g"));
    Serial.println(F("Rest the device flat first -- pitch/roll should read ~0."));
    Serial.println(F("Then tilt past 30 deg on either axis to trigger DIZZY flag."));
    Serial.println(F("accelX, accelY, accelZ (m/s^2) | pitch, roll (deg) | tiltDetected"));
}

void loop() {
    unsigned long now = millis();
    if (now - lastPrint < 200) return;
    lastPrint = now;

    int16_t rawX, rawY, rawZ;
    if (!readAccelRaw(rawX, rawY, rawZ)) {
        Serial.println(F("[WARN] Accel read failed -- check wiring/bus"));
        return;
    }

    float ax = (rawX / ACCEL_SCALE) * GRAVITY;
    float ay = (rawY / ACCEL_SCALE) * GRAVITY;
    float az = (rawZ / ACCEL_SCALE) * GRAVITY;

    float pitchDeg = atan2f(ay, az) * 180.0f / PI;
    float rollDeg  = atan2f(ax, az) * 180.0f / PI;
    bool tiltDetected = (fabsf(pitchDeg) > TILT_THRESHOLD_DEG ||
                         fabsf(rollDeg)  > TILT_THRESHOLD_DEG);

    Serial.print(ax, 2); Serial.print(F(", "));
    Serial.print(ay, 2); Serial.print(F(", "));
    Serial.print(az, 2); Serial.print(F(" | "));
    Serial.print(pitchDeg, 1); Serial.print(F(", "));
    Serial.print(rollDeg, 1); Serial.print(F(" | "));
    Serial.println(tiltDetected ? F("TILT!") : F("level"));
}
