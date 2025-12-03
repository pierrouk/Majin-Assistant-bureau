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

    // Mouvements Basiques
    void setAngle(int angle);
    int getAngle();
    
    // 💃 MOUVEMENTS AVANCÉS (Animations bloquantes)
    void moveSmooth(int targetAngle, int speedDelay); // Mouvement lent
    void shake(int repetitions, int range, int speedDelay); // Secousse

    // 🎭 CHOREGRAPHIES EMOTIONNELLES
    void animHappy();
    void animAngry();
    void animSad();
    void animSleep();
    void animWake();

    void testSequence();

private:
    Servo _servo;
    int _currentAngle;
    bool _enabled = true; 
    
    SemaphoreHandle_t _mutex;
    
    const int MIN_PULSE = 500;
    const int MAX_PULSE = 2400;
};

#endif