#include "InputManager.h"

InputManager::InputManager() {
}

GestureType InputManager::process(bool isTouched, int x, int y) {
    unsigned long currentTime = millis();
    GestureType detected = GESTURE_NONE;

    // --- 1. DÉBUT DU TOUCHER ---
    if (isTouched && !_wasTouched) {
        _startX = x;
        _startY = y;
        _touchStartTime = currentTime;
        _longPressTriggered = false;
        _wasTouched = true;
        // Serial.println("Input: Touch Start");
    }

    // --- 2. PENDANT LE TOUCHER (Check Long Press) ---
    else if (isTouched && _wasTouched) {
        // Si on appuie longtemps sans trop bouger
        if (!_longPressTriggered && (currentTime - _touchStartTime > LONG_PRESS_TIME)) {
            int deltaX = abs(x - _startX);
            int deltaY = abs(y - _startY);
            
            // On vérifie qu'on est resté à peu près au même endroit (tolerance 20px)
            if (deltaX < 20 && deltaY < 20) {
                _longPressTriggered = true;
                return GESTURE_LONG_PRESS; // ⚠️ Retour immédiat
            }
        }
    }

    // --- 3. RELÂCHEMENT (Analyse du Geste) ---
    else if (!isTouched && _wasTouched) {
        unsigned long duration = currentTime - _touchStartTime;
        int deltaX = x - _startX;
        int deltaY = y - _startY;
        int absX = abs(deltaX);
        int absY = abs(deltaY);

        _wasTouched = false;

        // Si on a déjà déclenché un Long Press, on ignore le relâchement
        if (_longPressTriggered) {
            return GESTURE_NONE;
        }

        // A. Est-ce un Swipe ? (Mouvement significatif)
        if (absX > SWIPE_THRESHOLD || absY > SWIPE_THRESHOLD) {
            // On détermine l'axe dominant
            if (absX > absY) {
                // Horizontal
                detected = (deltaX > 0) ? GESTURE_SWIPE_RIGHT : GESTURE_SWIPE_LEFT;
            } else {
                // Vertical
                detected = (deltaY > 0) ? GESTURE_SWIPE_DOWN : GESTURE_SWIPE_UP;
            }
        }
        // B. Est-ce un Tap ? (Court et stable)
        else if (duration < TAP_TIMEOUT) {
            detected = GESTURE_TAP;
        }
    }

    return detected;
}