#ifndef SOUND_SYSTEM_H
#define SOUND_SYSTEM_H

#include <Arduino.h>
#include "pins.h"
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

struct Note {
    uint16_t frequency;
    uint16_t duration;
};

class SoundSystem {
public:
    SoundSystem();
    bool begin();
    
    // 🛠️ GESTION ACTIVATION
    void setEnabled(bool enabled);
    bool isEnabled();

    void update(); 
    void tone(uint16_t freq, uint16_t duration);
    void mute();

    void playStartup();
    void playNotification();
    void playError();
    void playSuccess();

    bool isPlaying(); 

    void testSequence();

private:
    const int PWM_CHANNEL = 6; 
    const int RESOLUTION = 8;
    
    static const int MAX_NOTES = 20;
    Note _melodyQueue[MAX_NOTES];
    int _head = 0;
    int _tail = 0;
    int _notesCount = 0;

    unsigned long _lastNoteStart = 0;
    uint16_t _currentNoteDuration = 0;
    bool _isPlaying = false;
    
    // État global
    bool _enabled = true; // Par défaut

    SemaphoreHandle_t _mutex;

    void _enqueue(uint16_t freq, uint16_t duration);
};

#endif