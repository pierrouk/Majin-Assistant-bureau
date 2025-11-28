#include "SoundSystem.h"

SoundSystem::SoundSystem() {
    _mutex = xSemaphoreCreateMutex(); 
}

bool SoundSystem::begin() {
    ledcSetup(PWM_CHANNEL, 2000, RESOLUTION);
    ledcWrite(PWM_CHANNEL, 0); 

    pinMode(BUZZER_PIN, OUTPUT);
    digitalWrite(BUZZER_PIN, LOW);
    
    return true;
}

// 🛠️ GESTION ACTIVATION
void SoundSystem::setEnabled(bool enabled) {
    _enabled = enabled;
    if (!enabled) mute(); // Coupe le son immédiatement si on désactive
}

bool SoundSystem::isEnabled() {
    return _enabled;
}

void SoundSystem::mute() {
    xSemaphoreTake(_mutex, portMAX_DELAY);
    ledcDetachPin(BUZZER_PIN);
    pinMode(BUZZER_PIN, OUTPUT);
    digitalWrite(BUZZER_PIN, LOW);
    _notesCount = 0; 
    _head = 0; _tail = 0;
    _isPlaying = false;
    xSemaphoreGive(_mutex); 
}

void SoundSystem::_enqueue(uint16_t freq, uint16_t duration) {
    if (xSemaphoreTake(_mutex, 10) == pdTRUE) { 
        if (_notesCount < MAX_NOTES) {
            _melodyQueue[_head].frequency = freq;
            _melodyQueue[_head].duration = duration;
            _head = (_head + 1) % MAX_NOTES;
            _notesCount++;
        }
        xSemaphoreGive(_mutex);
    }
}

void SoundSystem::tone(uint16_t freq, uint16_t duration) {
    // 🛑 Si désactivé, on ne fait rien !
    if (!_enabled) return;
    
    mute();
    _enqueue(freq, duration);
}

bool SoundSystem::isPlaying() {
    return _isPlaying || (_notesCount > 0);
}

void SoundSystem::update() {
    // Si désactivé, on force l'arrêt propre
    if (!_enabled) {
        if (_isPlaying) {
             ledcWriteTone(PWM_CHANNEL, 0);
             ledcDetachPin(BUZZER_PIN);
             _isPlaying = false;
        }
        return;
    }

    unsigned long currentMillis = millis();

    if (_isPlaying) {
        if (currentMillis - _lastNoteStart >= _currentNoteDuration) {
            ledcWriteTone(PWM_CHANNEL, 0);
            ledcDetachPin(BUZZER_PIN);
            pinMode(BUZZER_PIN, OUTPUT);
            digitalWrite(BUZZER_PIN, LOW);
            _isPlaying = false;
            _lastNoteStart = currentMillis;
            _currentNoteDuration = 20; 
            return;
        }
    } 
    else {
        if (currentMillis - _lastNoteStart < _currentNoteDuration) return;

        bool hasNotes = false;
        Note n;
        
        if (xSemaphoreTake(_mutex, 5) == pdTRUE) {
            if (_notesCount > 0) {
                n = _melodyQueue[_tail];
                _tail = (_tail + 1) % MAX_NOTES;
                _notesCount--;
                hasNotes = true;
            }
            xSemaphoreGive(_mutex);
        }

        if (hasNotes) {
            if (n.frequency > 0) {
                ledcAttachPin(BUZZER_PIN, PWM_CHANNEL); 
                ledcWriteTone(PWM_CHANNEL, n.frequency);
            } else {
                ledcDetachPin(BUZZER_PIN);
                pinMode(BUZZER_PIN, OUTPUT);
                digitalWrite(BUZZER_PIN, LOW);
            }
            _lastNoteStart = currentMillis;
            _currentNoteDuration = n.duration;
            _isPlaying = true;
        }
    }
}

// --- Sons Prédéfinis ---
// Ils appellent tone() ou _enqueue(), donc ils respecteront le flag _enabled

void SoundSystem::playStartup() {
    if (!_enabled) return;
    tone(0, 1); 
    _enqueue(880, 100); _enqueue(988, 100); _enqueue(1175, 100); _enqueue(1319, 200);
}

void SoundSystem::playNotification() {
    if (!_enabled) return;
    _enqueue(2000, 80); _enqueue(0, 50); _enqueue(2500, 80);
}

void SoundSystem::playSuccess() {
    if (!_enabled) return;
    _enqueue(1000, 80); _enqueue(1500, 80); _enqueue(2000, 100);
}

void SoundSystem::playError() {
    if (!_enabled) return;
    tone(0, 1);
    _enqueue(400, 200); _enqueue(0, 50); _enqueue(300, 400);
}
void SoundSystem::testSequence() {
    if (!_enabled) return;
    tone(0, 1);
    _enqueue(262, 200); // C4
    _enqueue(294, 200); // D4
    _enqueue(330, 200); // E4
    _enqueue(349, 200); // F4
    _enqueue(392, 200); // G4
    _enqueue(440, 200); // A4
    _enqueue(494, 200); // B4
    _enqueue(523, 400); // C5
}