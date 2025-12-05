#include "DisplayDriver.h"

DisplayDriver::DisplayDriver() {
    _mutex = xSemaphoreCreateMutex();
}

bool DisplayDriver::begin() {
    xSemaphoreTake(_mutex, portMAX_DELAY);
    log_i("📺 [Display]: Démarrage...");

    if (!_tft.init()) {
        log_i("🔴 ERREUR [Display]: Échec init LGFX !");
        xSemaphoreGive(_mutex);
        return false;
    }

    _tft.setRotation(1);
    
    uint16_t calData[] = { 239, 309, 228, 0, 9, 316, 16, 0 };
    _tft.setTouchCalibrate(calData);

    _tft.setBrightness(250); 
    _tft.fillScreen(TFT_BLACK);
    
    // 🗑️ NETTOYAGE : On retire le texte de debug
    // _tft.setTextColor(TFT_WHITE);
    // _tft.setTextSize(2);
    // _tft.setCursor(10, 10);
    // _tft.println("Majin OS - Booting...");

    log_i("⚠️ [SD]: Désactivée pour permettre le boot");
    _sdInitialized = false;

    xSemaphoreGive(_mutex);
    return true;
}

LGFX_Majin* DisplayDriver::getLGFX() {
    return &_tft;
}

void DisplayDriver::secureDraw(std::function<void(LGFX_Majin*)> drawFunction) {
    if (xSemaphoreTake(_mutex, pdMS_TO_TICKS(50)) == pdTRUE) {
        drawFunction(&_tft);
        xSemaphoreGive(_mutex);
    } else {
        log_i("⚠️ [Display]: Ecran occupé");
    }
}

bool DisplayDriver::getTouchSafe(uint16_t* x, uint16_t* y) {
    bool touched = false;
    if (xSemaphoreTake(_mutex, pdMS_TO_TICKS(10)) == pdTRUE) {
        touched = _tft.getTouch(x, y);
        xSemaphoreGive(_mutex);
    }
    return touched;
}

void DisplayDriver::setBrightness(uint8_t level) {
    if (xSemaphoreTake(_mutex, 10) == pdTRUE) {
        _tft.setBrightness(level);
        xSemaphoreGive(_mutex);
    }
}

void DisplayDriver::setTouchCalibration(uint16_t* calData) {
    if (xSemaphoreTake(_mutex, 10) == pdTRUE) {
        _tft.setTouchCalibrate(calData);
        xSemaphoreGive(_mutex);
    }
}

bool DisplayDriver::_initSD() {
    // Code SD conservé mais désactivé
    return false;
}

void DisplayDriver::logTouch() {
    uint16_t x, y;
    if (xSemaphoreTake(_mutex, 10) == pdTRUE) {
        if (_tft.getTouch(&x, &y)) {
            Serial.printf("👆 [Touch]: X=%d Y=%d\n", x, y);
            _tft.fillCircle(x, y, 2, TFT_YELLOW);
        }
        xSemaphoreGive(_mutex);
    }
}