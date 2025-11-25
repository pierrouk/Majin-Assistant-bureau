#ifndef SERVO_MOTOR_H
#define SERVO_MOTOR_H

#include <Arduino.h>
#include <ESP32Servo.h>
#include "pins.h"
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

class ServoMotor {
public:
    ServoMotor();
    bool begin();

    // 🛠️ GESTION ACTIVATION
    void setEnabled(bool enabled);
    bool isEnabled();

    void setAngle(int angle);
    int getAngle();
    void testSequence();

private:
    Servo _servo;
    int _currentAngle;
    bool _enabled = true; // Par défaut
    
    SemaphoreHandle_t _mutex;
    
    const int MIN_PULSE = 500;
    const int MAX_PULSE = 2400;
};

#endif