#include "LightSensor.h"

LightSensor::LightSensor() {
    _isInitialized = false;
}

bool LightSensor::begin(TwoWire *wirePtr) {
    // Note : On ne fait PAS Wire.begin() ici. 
    // On suppose que le bus est déjà démarré dans le main.cpp (Architecture propre)
    
    // On tente de démarrer le VEML sur le bus spécifié
    if (!_veml.begin(wirePtr)) {
        log_i("🔴 ERREUR [LightSensor]: VEML7700 introuvable !");
        _isInitialized = false;
        return false;
    }

    log_i("🟢 [LightSensor]: VEML7700 connecté.");
    
    // Configuration pour un environnement de bureau (Lumière artificielle + Naturelle)
    configureAutoRange();

    _isInitialized = true;
    return true;
}

void LightSensor::configureAutoRange() {
    // Gain : 1/8 (Bon pour éviter la saturation en lumière intérieure forte)
    _veml.setGain(VEML7700_GAIN_2); 
    
    // Integration Time : 100ms (Réactif)
    _veml.setIntegrationTime(VEML7700_IT_100MS);

    // (Optionnel) On pourrait activer les interruptions ici plus tard
}

float LightSensor::getLux() {
    if (!_isInitialized) {
        return -1.0; // Code d'erreur standard
    }

    // Lecture brute
    float lux = _veml.readLux();

    // Petit filet de sécurité : Si la lecture échoue, la librairie renvoie parfois des valeurs absurdes
    if (isnan(lux)) {
        return -1.0;
    }

    return lux;
}