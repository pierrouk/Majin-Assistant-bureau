#ifndef ENVIRONMENT_SENSOR_H
#define ENVIRONMENT_SENSOR_H

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_AHTX0.h>
#include "pins.h"

struct EnvData {
    float temperature;
    float humidity;
    bool valid;
};

class EnvironmentSensor {
public:
    EnvironmentSensor();

    /**
     * @brief Initialise le capteur sur le bus I2C SECONDAIRE (Wire1)
     */
    bool begin();

    /**
     * @brief Lit les données (non-bloquant si possible, mais AHT est rapide)
     */
    EnvData read();

private:
    Adafruit_AHTX0 _aht;
    bool _initialized = false;
};

#endif