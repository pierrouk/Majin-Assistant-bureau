#include "HapticDriver.h"

HapticDriver::HapticDriver(int pin) {
    _pin = pin;
}

void HapticDriver::begin() {
    pinMode(_pin, OUTPUT);
    digitalWrite(_pin, LOW);
}

void HapticDriver::setEnabled(bool enabled) {
    _enabled = enabled;
}

void HapticDriver::vibrate(int duration) {
    if (!_enabled) return;
    
    // On met la patte à HIGH (3.3V)
    // Comme ton module a un MOSFET, c'est suffisant pour déclencher le 5V du moteur.
    digitalWrite(_pin, HIGH);
    
    // Délai bloquant (nécessaire ici pour garantir la durée)
    delay(duration); 
    
    digitalWrite(_pin, LOW);
}

void HapticDriver::click() {
    // ⚠️ CORRECTIF : On passe de 15ms à 80ms
    // Pour vaincre l'inertie du moteur au démarrage
    vibrate(80); 
}

void HapticDriver::buzz() {
    vibrate(200); 
}

void HapticDriver::doubleBuzz() {
    if (!_enabled) return;
    vibrate(150);
    delay(100);
    vibrate(150);
}