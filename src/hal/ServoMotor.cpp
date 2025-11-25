#include "ServoMotor.h"

ServoMotor::ServoMotor() {
    _currentAngle = 90; 
    _mutex = xSemaphoreCreateMutex();
}

bool ServoMotor::begin() {
    xSemaphoreTake(_mutex, portMAX_DELAY);

    _servo.setPeriodHertz(50);
    
    // On n'attache que si activé
    if (_enabled) {
        if (_servo.attach(SERVO_PIN, MIN_PULSE, MAX_PULSE) != -1) {
            Serial.printf("🟢 [ServoMotor]: Attaché sur GPIO %d\n", SERVO_PIN);
            _servo.write(90);
        }
    }
    
    xSemaphoreGive(_mutex);
    delay(600); 
    return true;
}

// 🛠️ GESTION ACTIVATION
void ServoMotor::setEnabled(bool enabled) {
    if (xSemaphoreTake(_mutex, portMAX_DELAY) == pdTRUE) {
        _enabled = enabled;
        if (!_enabled) {
            // Si on désactive, on détache pour couper le couple
            _servo.detach();
            Serial.println("🛑 [ServoMotor]: Détaché (Disabled)");
        } else {
            // Si on active, on rattache
            if (!_servo.attached()) {
                _servo.attach(SERVO_PIN, MIN_PULSE, MAX_PULSE);
                _servo.write(_currentAngle); // On revient à la dernière position
                Serial.println("🟢 [ServoMotor]: Ré-attaché (Enabled)");
            }
        }
        xSemaphoreGive(_mutex);
    }
}

bool ServoMotor::isEnabled() {
    return _enabled;
}

void ServoMotor::setAngle(int angle) {
    if (angle < 0) angle = 0;
    if (angle > 180) angle = 180;

    if (xSemaphoreTake(_mutex, (TickType_t)10) == pdTRUE) {
        // 🛑 Si désactivé, on met juste à jour la variable "virtuelle" mais on ne bouge pas
        _currentAngle = angle;
        
        if (_enabled && _servo.attached()) {
            _servo.write(angle);
        }
        xSemaphoreGive(_mutex);
    }
}

int ServoMotor::getAngle() {
    int angleCopy = 0;
    if (xSemaphoreTake(_mutex, (TickType_t)10) == pdTRUE) {
        angleCopy = _currentAngle;
        xSemaphoreGive(_mutex);
    }
    return angleCopy;
}

void ServoMotor::testSequence() {
    // Si désactivé, on force temporairement l'activation pour le test
    bool wasEnabled = _enabled;
    if (!wasEnabled) setEnabled(true);

    Serial.println("🔧 [ServoMotor]: Test Sequence");
    setAngle(0); delay(800);
    setAngle(180); delay(800);
    setAngle(90); delay(500);

    // On remet l'état d'origine
    if (!wasEnabled) setEnabled(false);
}