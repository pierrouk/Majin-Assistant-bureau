#ifndef USB_MANAGER_H
#define USB_MANAGER_H

#include <Arduino.h>
#include "USB.h"
#include "USBHIDKeyboard.h"
#include "USBHIDMouse.h"
#include "USBHIDConsumerControl.h" 

class UsbManager {
public:
    UsbManager();

    void begin();
    bool isConnected();

    // --- Fonctions Souris ---
    void moveMouse(int x, int y);
    void clickMouse();
    
    // 🛠️ CORRECTION : Réintégration de la méthode manuelle
    void jiggleMouse(); 

    // --- Gestion du Jiggler Automatique (Tâche de fond) ---
    void setJiggler(bool active);
    bool isJigglerActive();
    void update(); 

    // --- Fonctions Stream Deck ---
    void sendShortcut(uint8_t key, bool ctrl, bool shift, bool alt);
    void commandOBSStudio();
    void commandMute();
    void commandCamera();
    void commandChat();

private:
    USBHIDKeyboard _keyboard;
    USBHIDMouse _mouse;
    USBHIDConsumerControl _consumerControl; 
    
    bool _isReady = false;
    
    // Jiggler Logic
    bool _jigglerActive = false;
    unsigned long _lastJiggleTime = 0;
    int _jiggleStep = 0;
};

#endif