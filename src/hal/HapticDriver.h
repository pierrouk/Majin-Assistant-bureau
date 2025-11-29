#ifndef HAPTIC_DRIVER_H
#define HAPTIC_DRIVER_H

#include <Arduino.h>

class HapticDriver {
public:
    // Constructeur avec la pin cible
    HapticDriver(int pin);

    // Initialisation
    void begin();

    // Active/Désactive le retour haptique
    void setEnabled(bool enabled);

    // --- Patterns de Vibration ---
    
    // Un "Tic" très court (pour le tactile)
    void click();

    // Une vibration moyenne (pour une notif)
    void buzz();

    // Une double vibration (pour une erreur ou réveil)
    void doubleBuzz();

    // Vibration manuelle
    void vibrate(int duration);

private:
    int _pin;
    bool _enabled = true;
};

#endif