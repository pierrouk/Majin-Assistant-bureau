#include "EnvironmentSensor.h"

EnvironmentSensor::EnvironmentSensor() {
}

bool EnvironmentSensor::begin() {
    log_i("🌡️ [EnvSensor]: Démarrage AHT10...");
    
    // On utilise l'instance globale Wire (déjà initialisée sur 5/6 dans le main)
    // Pas besoin de refaire Wire.begin() ici.
    
    if (!_aht.begin(&Wire)) {
        log_i("🔴 ERREUR [EnvSensor]: AHT10 non trouvé !");
        log_i("   -> Vérifiez le câblage sur les pins 5 (SDA) et 6 (SCL)");
        _initialized = false;
        return false;
    }

    log_i("🟢 [EnvSensor]: AHT10 Connecté.");
    _initialized = true;
    return true;
}

EnvData EnvironmentSensor::read() {
    EnvData data = {0.0f, 0.0f, false};
    
    if (!_initialized) return data;

    sensors_event_t humidity, temp;
    _aht.getEvent(&humidity, &temp);
    
    data.temperature = temp.temperature;
    data.humidity = humidity.relative_humidity;
    data.valid = true;

    return data;
}