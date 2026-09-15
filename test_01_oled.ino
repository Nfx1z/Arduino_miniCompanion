// ============================================================================
//  MODULE TEST 1: SH1106 OLED Display
//  Verifies: I2C init, display alloc, basic drawing primitives, text render
//  Wiring: SDA -> GPIO6, SCL -> GPIO7, addr 0x3C
// ============================================================================

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>

#define PIN_I2C_SDA   6
#define PIN_I2C_SCL   7
#define SCREEN_WIDTH  128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
#define OLED_I2C_ADDR 0x3C

Adafruit_SH1106G display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

unsigned long lastSwap = 0;
uint8_t frame = 0;

void drawTestFrame(uint8_t f) {
    display.clearDisplay();
    display.setTextColor(SH110X_WHITE);

    switch (f) {
        case 0:
            // Text render test
            display.setTextSize(1);
            display.setCursor(0, 0);
            display.println(F("OLED TEST OK"));
            display.setCursor(0, 12);
            display.println(F("Text render pass"));
            break;
        case 1:
            // Shape test -- rectangle + circle + line
            display.drawRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, SH110X_WHITE);
            display.drawCircle(64, 32, 20, SH110X_WHITE);
            display.drawLine(0, 0, SCREEN_WIDTH - 1, SCREEN_HEIGHT - 1, SH110X_WHITE);
            break;
        case 2:
            // Full white / full black flash -- checks for dead pixels / ghosting
            display.fillScreen(SH110X_WHITE);
            break;
        case 3:
            display.fillScreen(SH110X_BLACK);
            display.setCursor(30, 28);
            display.setTextSize(2);
            display.println(F("PASS"));
            break;
    }
    display.display();
}

void setup() {
    Serial.begin(115200);
    delay(300);
    Serial.println(F("\n--- OLED Module Test ---"));

    Wire.begin(PIN_I2C_SDA, PIN_I2C_SCL);
    Wire.setClock(400000);

    if (!display.begin(OLED_I2C_ADDR, true)) {
        Serial.println(F("[FAIL] SH1106 allocation failed. Check wiring / address."));
        while (true) { yield(); }
    }
    Serial.println(F("[OK] Display initialized at 0x3C"));
    Serial.println(F("Cycling: text -> shapes -> white flash -> PASS, every 2s"));

    display.clearDisplay();
    display.display();
}

void loop() {
    unsigned long now = millis();
    if (now - lastSwap >= 2000) {
        lastSwap = now;
        drawTestFrame(frame);
        Serial.print(F("Showing frame index: "));
        Serial.println(frame);
        frame = (frame + 1) % 4;
    }
}
