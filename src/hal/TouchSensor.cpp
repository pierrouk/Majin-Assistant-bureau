#include "TouchSensor.h"

TouchSensor::TouchSensor() {
}

bool TouchSensor::begin() {
    // Configuration pour un module externe (TTP223 par exemple)
    // La plupart envoient un signal HIGH quand touchés.
    // Si le tien envoie LOW, change juste la logique dans _readHardware.
    pinMode(TOUCH_BUTTON_PIN, INPUT);
    
    Serial.printf("👆 [TouchSensor]: Module Numérique initialise sur GPIO %d\n", TOUCH_BUTTON_PIN);
    return true;
}

bool TouchSensor::_readHardware() {
    // Lit l'état numérique. 
    // TTP223 standard : HIGH = Touché, LOW = Pas touché
    return digitalRead(TOUCH_BUTTON_PIN) == HIGH;
}

bool TouchSensor::isTouched() {
    return _isPressed;
}

void TouchSensor::update() {
    unsigned long currentTime = millis();
    bool reading = _readHardware();

    // --- 1. GESTION DU DEBOUNCE (Anti-Rebond) ---
    // Si la lecture change (bruit ou vrai appui), on reset le chrono
    if (reading != _lastPinState) {
        _lastDebounceTime = currentTime;
    }
    _lastPinState = reading;

    // Si l'état est stable depuis assez longtemps
    if ((currentTime - _lastDebounceTime) > DEBOUNCE_DELAY) {
        
        // Si l'état stable a changé (Front Montant ou Descendant confirmé)
        if (reading != _isPressed) {
            _isPressed = reading;

            // --- 2. LOGIQUE D'ÉVÉNEMENTS ---
            
            if (_isPressed) {
                // FRONT MONTANT (Appui commence)
                _pressStartTime = currentTime;
            } 
            else {
                // FRONT DESCENDANT (Relâchement)
                unsigned long duration = currentTime - _pressStartTime;

                // A. Appui Long
                if (duration > LONG_PRESS_DURATION) {
                    _detectedEvent = TOUCH_LONG;
                    _tapCount = 0; // Reset double tap
                }
                // B. Appui Court (Gestion Tap/Double Tap)
                else {
                    if (_tapCount == 0) {
                        _tapCount = 1;
                        _lastTapTime = currentTime;
                    } else {
                        // Deuxième tap ?
                        if (currentTime - _lastTapTime < DOUBLE_TAP_TIMEOUT) {
                            _detectedEvent = TOUCH_DOUBLE;
                            _tapCount = 0;
                        } else {
                            // Trop tard, on redémarre un cycle
                            _tapCount = 1;
                            _lastTapTime = currentTime;
                        }
                    }
                }
            }
        }
    }

    // --- 3. TIMEOUT SINGLE TAP ---
    // Si on attend le 2ème tap mais qu'il ne vient pas
    if (_tapCount == 1 && (currentTime - _lastTapTime >= DOUBLE_TAP_TIMEOUT)) {
        _detectedEvent = TOUCH_SINGLE;
        _tapCount = 0;
    }
}

TouchEventType TouchSensor::getEvent() {
    TouchEventType evt = _detectedEvent;
    _detectedEvent = TOUCH_NONE;
    return evt;
}