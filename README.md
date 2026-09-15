# Arduino Mini Companion

An interactive ESP32-based animatronic companion robot with emotion states, gesture recognition, temperature sensing, and dual-mode operation (Robot Mode + MP3 Player).

## What This Is

The **Arduino Mini Companion** is an autonomous robotic companion that responds to human interaction through touch, motion, and environmental stimuli. It displays animated emotions on an OLED screen, plays audio feedback, and can operate in two modes: an interactive emotion-driven Robot Mode or a simple MP3 music player. Perfect for a tabletop pet or interactive installation.

### Stack

- **Language:** C++ (Arduino)
- **Microcontroller:** ESP32-C3 Mini
- **Display:** 128×64 OLED (SH1106 driver)
- **Audio:** DFPlayer Mini MP3 module (SD card)
- **Sensors:** 
  - MPU6050 (accelerometer/gyroscope for tilt detection)
  - SHT30D (temperature & humidity)
  - PIR motion sensor
  - TTP223 capacitive touch pads (×3)
- **Communications:** I2C, UART

## How It's Organized

Ino_Robot.ino Main sketch (~1300 lines) — state machine, input handling, animation engine, mode control

animation.h Bitmap frame data (19 pre-rendered OLED frames) and timing constants for animation playback

test_*.ino Individual sensor/module tests:

- test_01_oled.ino OLED display test
- test_02_mpu6050.ino MPU6050 accelerometer test
- test_03_sht30d.ino Temperature/humidity sensor test
- test_04_ttp223_rub.ino Touch pad test
- test_05_pir.ino PIR motion sensor test
- test_06_dfplayer.ino DFPlayer audio module test
- spec.txt Hardware pinouts, component dimensions, animation frame timing specs, and behavior documentation
- `companion_robot.txt`: Duplication of main sketch for future reference
- `frame/` : Based image for the frame of the companion mode using OLED pixel editor
- `img/` : Physical output of the companion 

### How It Fits Together

**Robot Mode** operates a state machine with 10 states (Sleep, Plain, Angry, Sad, Happy, Dizzy, Hot, Freeze, Wake-Rub, Temp-Cooldown). The device wakes from sleep on motion (PIR) or touch, then cycles through emotional responses based on rubbing duration, temperature extremes, or tilt. Each state triggers a specific OLED animation + audio track. Rub duration on capacitive pads (TTP223) accumulates to trigger transitions. The MPU6050 detects sudden tilts (>30°) to force the Dizzy state. SHT30D temperature readings trigger Hot (>33°C) or Freeze (<20°C) states.

**MP3 Mode** (toggled by holding the Mode button for 5s) converts the device into a music player. The two rub pads become volume up/down and track navigation. Long-press ramps volume; short-press skips tracks. The Mode button double-tap toggles loop behavior.

## How to Run It

### Hardware Setup

1. **Assemble** per `spec.txt` pinout:
   - SDA → GPIO 6, SCL → GPIO 7 (I2C)
   - DFPlayer RX → GPIO 1, TX → GPIO 0 (UART)
   - PIR → GPIO 4
   - TTP223 (Rub) → GPIO 5, GPIO 10
   - TTP223 (Mode) → GPIO 3

2. **Prepare SD Card** for DFPlayer (insert into module):
   - `/01/` folder: numbered tracks (001.mp3–012.mp3) for Robot mode sounds
   - `/02/` folder: numbered tracks (001.mp3+) for MP3 mode music
   - File count in `/02/` is auto-detected on each MP3 entry

3. **Flash** `Ino_Robot.ino` to ESP32-C3 Mini at **115200 baud**

### To Test Components Individually

Flash one of the `test_*.ino` sketches and open Serial Monitor (115200 baud):

```bash
# Test OLED display
Upload: test_01_oled.ino

# Test motion/tilt sensor
Upload: test_02_mpu6050.ino
# or for diagnostics:
Upload: test_02b_mpu_whoami_diagnostic.ino

# Test temperature/humidity
Upload: test_03_sht30d.ino

# Test capacitive touch pads
Upload: test_04_ttp223_rub.ino

# Test motion detection
Upload: test_05_pir.ino

# Test MP3 audio
Upload: test_06_dfplayer.ino
# or for diagnostics:
Upload: test_06b_dfplayer_raw_diag.ino
To Run the Full Robot
Upload Ino_Robot.ino
Watch Serial output (115200 baud) for boot messages:
Code
--- Companion Boot (v2) ---
[OK ] OLED SH1106
[OK ] DFPlayer
[OK ] MPU6500 woken + configured
[OK ] SHT30D
--- Ready ---
Device enters Sleep (loops sleeping animation, plays sleep.mp3)
Interact:
Motion detected → enters Plain state, plays greeting
Rub for 4s → enters Happy (from Sleep)
Rub for 2s → enters Happy (from Plain)
Tilt >30° → enters Dizzy (from any state)
Hold Mode button 5s → toggles Robot ↔ MP3 mode
