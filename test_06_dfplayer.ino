// ============================================================================
//  DIAGNOSTIC: Raw UART probe for DFPlayer Mini on GPIO0 (RX) / GPIO1 (TX)
//  Bypasses DFRobotDFPlayerMini entirely so we can see whether ANY bytes
//  come back from the module, independent of the library's own timing/ACK
//  assumptions. Helps separate "wiring/power problem" from "library problem".
// ============================================================================

#include <HardwareSerial.h>

#define PIN_DFPLAYER_RX 0   // ESP32 RX <- DFPlayer TX
#define PIN_DFPLAYER_TX 1   // ESP32 TX -> DFPlayer RX

HardwareSerial dfSerial(1);

void dumpAvailableBytes(unsigned long windowMs) {
    unsigned long start = millis();
    bool gotAny = false;
    while (millis() - start < windowMs) {
        if (dfSerial.available()) {
            gotAny = true;
            uint8_t b = dfSerial.read();
            Serial.print(F("  RX byte: 0x"));
            if (b < 0x10) Serial.print('0');
            Serial.println(b, HEX);
        }
    }
    if (!gotAny) {
        Serial.println(F("  (nothing received in this window)"));
    }
}

void setup() {
    Serial.begin(115200);
    delay(300);
    Serial.println(F("\n--- DFPlayer RAW UART Diagnostic ---"));
    Serial.println(F("This does NOT use the DFRobotDFPlayerMini library."));
    Serial.println(F("It only opens the UART and reports raw bytes, so we can"));
    Serial.println(F("tell a wiring/power problem apart from a library timing issue."));

    dfSerial.begin(9600, SERIAL_8N1, PIN_DFPLAYER_RX, PIN_DFPLAYER_TX);

    Serial.println(F("\n[1] Waiting 2s for power-up, then listening 3s for any"));
    Serial.println(F("    unsolicited bytes (some DFPlayer clones auto-report"));
    Serial.println(F("    SD card status shortly after boot)..."));
    delay(2000);
    dumpAvailableBytes(3000);

    Serial.println(F("\n[2] Sending a properly-checksummed RESET command"));
    Serial.println(F("    (7E FF 06 0C 00 00 00 FE EF EF) and listening 2s..."));
    uint8_t resetCmd[10] = {0x7E, 0xFF, 0x06, 0x0C, 0x00, 0x00, 0x00, 0xFE, 0xEF, 0xEF};
    dfSerial.write(resetCmd, sizeof(resetCmd));
    dumpAvailableBytes(2000);

    Serial.println(F("\n[3] Sending a 'query current volume' command"));
    Serial.println(F("    (7E FF 06 43 00 00 00 FE B4 EF) and listening 2s..."));
    uint8_t queryVol[10] = {0x7E, 0xFF, 0x06, 0x43, 0x00, 0x00, 0x00, 0xFE, 0xB4, 0xEF};
    dfSerial.write(queryVol, sizeof(queryVol));
    dumpAvailableBytes(2000);

    Serial.println(F("\n--- Interpreting results ---"));
    Serial.println(F("Got 0x7E ... 0xEF framed bytes back in [2] or [3]?"));
    Serial.println(F("  -> Module IS responding on this wiring/pins."));
    Serial.println(F("     The library-level begin() failure is then a timing/"));
    Serial.println(F("     ACK-flag issue, not wiring -- try increasing the delay"));
    Serial.println(F("     before dfPlayer.begin() to 2-3s, or call"));
    Serial.println(F("     dfPlayer.begin(dfSerial, false, true) to relax ACK checking."));
    Serial.println(F(""));
    Serial.println(F("Got NOTHING in all three windows?"));
    Serial.println(F("  -> No data is reaching GPIO0 from the DFPlayer at all. Check:"));
    Serial.println(F("     - DFPlayer TX wired to GPIO0, DFPlayer RX wired to GPIO1"));
    Serial.println(F("       (not swapped -- this is the single most common mistake)"));
    Serial.println(F("     - DFPlayer has its own stable 5V (or 3.3-5V per your module),"));
    Serial.println(F("       not powered only through the ESP32's 3.3V regulator"));
    Serial.println(F("     - Common GND between ESP32 and DFPlayer"));
    Serial.println(F("     - microSD card is FAT32, actually seated, and has files on it"));
    Serial.println(F("       (some DFPlayer clones refuse to respond to ANYTHING if they"));
    Serial.println(F("        can't see a valid card)"));
    Serial.println(F("     - Try swapping in a different microSD card entirely"));
}

void loop() {
    // Pass through anything further received, in case you want to probe more
    // commands manually by editing setup() and re-flashing.
    if (dfSerial.available()) {
        uint8_t b = dfSerial.read();
        Serial.print(F("RX: 0x"));
        if (b < 0x10) Serial.print('0');
        Serial.println(b, HEX);
    }
}
