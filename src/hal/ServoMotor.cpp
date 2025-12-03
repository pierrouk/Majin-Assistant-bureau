#include "ServoMotor.h"

ServoMotor::ServoMotor() {
    _currentAngle = 90; 
    _mutex = xSemaphoreCreateMutex();
}

bool ServoMotor::begin() {
    xSemaphoreTake(_mutex, portMAX_DELAY);

    _servo.setPeriodHertz(50);
    
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

void ServoMotor::setEnabled(bool enabled) {
    if (xSemaphoreTake(_mutex, portMAX_DELAY) == pdTRUE) {
        _enabled = enabled;
        if (!_enabled) {
            _servo.detach();
            log_i("🛑 [ServoMotor]: Détaché (Disabled)");
        } else {
            if (!_servo.attached()) {
                _servo.attach(SERVO_PIN, MIN_PULSE, MAX_PULSE);
                _servo.write(_currentAngle); 
                log_i("🟢 [ServoMotor]: Ré-attaché (Enabled)");
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

// 💃 MOUVEMENT FLUIDE (Pour Sad/Sleep)
// speedDelay : temps en ms entre chaque degré (plus c'est grand, plus c'est lent)
void ServoMotor::moveSmooth(int target, int speedDelay) {
    if (!_enabled) return;
    int start = _currentAngle;
    int step = (start < target) ? 1 : -1;
    
    // On avance degré par degré
    for (int i = start; i != target; i += step) {
        setAngle(i); // Utilise la méthode thread-safe
        vTaskDelay(pdMS_TO_TICKS(speedDelay)); 
    }
    setAngle(target); // Assure la position finale
}

// 💃 SECOUSSE (Pour Happy/Angry)
void ServoMotor::shake(int reps, int range, int speedDelay) {
    if (!_enabled) return;
    int center = 90; // On secoue toujours autour du centre pour l'instant
    
    for (int i = 0; i < reps; i++) {
        setAngle(center + range);
        vTaskDelay(pdMS_TO_TICKS(speedDelay));
        setAngle(center - range);
        vTaskDelay(pdMS_TO_TICKS(speedDelay));
    }
    setAngle(center); // Retour au calme
}

// 🎭 CHOREGRAPHIES ---------------------

void ServoMotor::animHappy() {
    // Petits frétillements rapides (Excitation)
    // 3 allers-retours, amplitude 15°, très rapide (80ms)
    shake(3, 15, 80);
}

void ServoMotor::animAngry() {
    // Grand "NON" violent
    // 2 allers-retours, amplitude 40°, vitesse moyenne (120ms)
    shake(2, 40, 120);
}

void ServoMotor::animSad() {
    // Baisse la tête lentement (ou regarde sur le côté tristement)
    // On va vers 130° (supposons que c'est "bas" ou "loin") très doucement
    moveSmooth(130, 25); 
    vTaskDelay(pdMS_TO_TICKS(1000)); // Reste triste 1s
    moveSmooth(90, 20); // Revient doucement
}

void ServoMotor::animSleep() {
    // S'effondre doucement vers une position de repos (ex: 150°)
    moveSmooth(150, 40); // Très lent
}

void ServoMotor::animWake() {
    // Relève la tête énergiquement
    moveSmooth(90, 10); // Assez rapide
}

void ServoMotor::testSequence() {
    bool wasEnabled = _enabled;
    if (!wasEnabled) setEnabled(true);

    log_i("🔧 [Servo]: Test Happy");
    animHappy(); delay(500);
    
    log_i("🔧 [Servo]: Test Sad");
    animSad(); delay(500);

    if (!wasEnabled) setEnabled(false);
}