#ifndef BOOT_SEQUENCE_H
#define BOOT_SEQUENCE_H

#include <Arduino.h>
#include "../hal/ServoMotor.h"
#include "../hal/SoundSystem.h"
#include "../hal/DisplayDriver.h"
#include "SettingsManager.h"
#include "UIManager.h"
#include "CoreManager.h"
#include "NetworkManager.h"

class BootSequence {
public:
    void run(ServoMotor* head, SoundSystem* voice, DisplayDriver* display, 
             SettingsManager* settings, UIManager* ui, 
             CoreManager* core, NetworkManager* net);

private:
    void _waitSafe(unsigned long ms, SoundSystem* voice);
    
    // Moteur unique Audio/Vidéo synchronisé
    void _playIntroSync(LGFX_Majin* tft, SoundSystem* voice, ServoMotor* head);

    // 🆕 Helper pour l'effet Arc-en-ciel (Sprint 2)
    uint16_t _rainbowColor(uint8_t index);
};

#endif