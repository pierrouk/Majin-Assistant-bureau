#ifndef TOUCH_SENSOR_H
#define TOUCH_SENSOR_H

#include <Arduino.h>
#include "pins.h"

enum TouchEventType {
    TOUCH_NONE,
    TOUCH_SINGLE,
    TOUCH_DOUBLE,
    TOUCH_LONG
};

class TouchSensor {
public:
    TouchSensor();

    /**
     * @brief Initialise le module tactile (Mode Digital)
     * @return true toujours (pas de calibration nécessaire sur module externe)
     */
    bool begin();

    /**
     * @brief Machine à états, à appeler le plus souvent possible
     */
    void update();

    TouchEventType getEvent();
    
    // Debug : Voir l'état brut
    bool isTouched();

private:
    // Paramètres temporels (Ajustés pour être "snappy")
    const int DEBOUNCE_DELAY = 50;
    const int DOUBLE_TAP_TIMEOUT = 400; 
    const int LONG_PRESS_DURATION = 800; 

    // États
    TouchEventType _detectedEvent = TOUCH_NONE;
    unsigned long _pressStartTime = 0;
    unsigned long _lastTapTime = 0;
    int _tapCount = 0;
    
    bool _isPressed = false;      // État stable (après debounce)
    bool _lastPinState = false;   // État précédent de la pin (pour debounce)
    unsigned long _lastDebounceTime = 0; // Timer debounce

    // Lecture hardware
    bool _readHardware();
};

#endif