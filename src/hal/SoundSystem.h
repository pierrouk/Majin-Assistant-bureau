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
    
    // GESTION ACTIVATION
    void setEnabled(bool enabled);
    bool isEnabled();

    void update(); 
    void tone(uint16_t freq, uint16_t duration);
    void mute();

    // 🔊 SONOTHÈQUE R2D2
    void playStartup();
    void playNotification(); // Bip simple
    void playSuccess();      // Confirmation
    void playError();        // Erreur grave
    
    // Nouvelles émotions
    void playHappy();        // Gazouillis montant
    void playSad();          // Descente lente
    void playAngry();        // Bruit sourd rapide
    void playSurprised();    // Montée très rapide
    void playBored();        // Soupir (notes longues basses)

    bool isPlaying(); 
    void testSequence();

private:
    const int PWM_CHANNEL = 6; 
    const int RESOLUTION = 8;
    
    static const int MAX_NOTES = 30; // ⚠️ Augmenté pour les séquences R2D2
    Note _melodyQueue[MAX_NOTES];
    int _head = 0;
    int _tail = 0;
    int _notesCount = 0;

    unsigned long _lastNoteStart = 0;
    uint16_t _currentNoteDuration = 0;
    bool _isPlaying = false;
    
    bool _enabled = true; 
    SemaphoreHandle_t _mutex;

    void _enqueue(uint16_t freq, uint16_t duration);
};

#endif