#ifndef PINS_H
#define PINS_H

// --- 📺 ÉCRAN TFT (Validé par le test Damier) ---
#define PIN_SCLK        12
#define PIN_MOSI        11
#define PIN_MISO        14
#define PIN_TFT_CS      10
#define PIN_TFT_DC      9
#define PIN_TFT_RST     8
#define PIN_TFT_BL      7

// --- 💾 CARTE SD (Désactivée pour l'instant) ---
#define PIN_SD_CS       18

// --- 🌐 BUS I2C ---
#define ESP32_I2C_SDA_PIN 5
#define ESP32_I2C_SCL_PIN 6

// Alias
#define PIN_AHT_SDA     ESP32_I2C_SDA_PIN
#define PIN_AHT_SCL     ESP32_I2C_SCL_PIN
#define PIN_FT_SDA      16
#define PIN_FT_SCL      15

#define PIN_FT_RST      4
#define PIN_FT_INT      17

// --- 🤖 PÉRIPHÉRIQUES ---
#define SERVO_PIN        42
#define TOUCH_BUTTON_PIN 13
#define BUZZER_PIN       21
// ⬅️ NOUVEAU : Vibreur (Recyclage de l'ancienne pin 41)
#define VIBE_PIN         46
#endif // PINS_H