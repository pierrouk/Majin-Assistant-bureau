#ifndef INPUT_MANAGER_H
#define INPUT_MANAGER_H

#include <Arduino.h>

// Les types de gestes reconnus
enum GestureType {
    GESTURE_NONE,
    GESTURE_TAP,
    GESTURE_DOUBLE_TAP, // (Réservé pour futur usage)
    GESTURE_LONG_PRESS,
    GESTURE_SWIPE_UP,
    GESTURE_SWIPE_DOWN,
    GESTURE_SWIPE_LEFT,
    GESTURE_SWIPE_RIGHT
};

class InputManager {
public:
    InputManager();

    /**
     * @brief Analyse l'état tactile actuel et retourne un geste si détecté.
     * A appeler à chaque frame (~30-50ms).
     * * @param isTouched true si l'écran est touché actuellement
     * @param x coordonnée X actuelle
     * @param y coordonnée Y actuelle
     * @return GestureType Le geste détecté (ou GESTURE_NONE)
     */
    GestureType process(bool isTouched, int x, int y);

private:
    // Configuration (Seuils)
    const int SWIPE_THRESHOLD = 50;    // Pixels min pour considérer un swipe
    const int TAP_TIMEOUT = 400;       // Ms max pour un tap rapide
    const int LONG_PRESS_TIME = 800;   // Ms min pour appui long

    // État interne
    bool _wasTouched = false;
    unsigned long _touchStartTime = 0;
    int _startX = 0;
    int _startY = 0;
    
    // Pour éviter de redéclencher un long press en boucle
    bool _longPressTriggered = false; 
};

#endif