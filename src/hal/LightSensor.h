#ifndef LIGHT_SENSOR_H
#define LIGHT_SENSOR_H

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_VEML7700.h>

class LightSensor {
public:
    LightSensor();

    /**
     * @brief Initialise le capteur
     * @param wirePtr Pointeur vers l'instance I2C (ex: &Wire ou &Wire1)
     * @return true si succès, false si capteur absent
     */
    bool begin(TwoWire *wirePtr = &Wire);

    /**
     * @brief Récupère la luminosité en Lux
     * @return float valeur en Lux, ou -1.0 si erreur
     */
    float getLux();

private:
    Adafruit_VEML7700 _veml;
    bool _isInitialized;
    
    // Configure le gain et le temps d'intégration pour un usage bureau
    void configureAutoRange(); 
};

#endif // LIGHT_SENSOR_H