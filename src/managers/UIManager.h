#ifndef UI_MANAGER_H
#define UI_MANAGER_H

#include <Arduino.h>
#include <LovyanGFX.hpp>
#include "../hal/DisplayDriver.h"
#include "CoreManager.h"
#include "NetworkManager.h"
#include "SettingsManager.h" 
#include "UsbManager.h" 

// 📦 TYPES PARTAGÉS
#include "../include/SharedTypes.h"

// 🎨 RENDERERS
#include "../renderers/FaceRenderer.h" 
#include "../renderers/CountdownRenderer.h"
#include "../renderers/DeckRenderer.h"
#include "../renderers/MenuRenderer.h"
#include "../renderers/OverlayRenderer.h" // ⬅️ NOUVEAU

// Particules (Restent ici pour l'instant)
struct Particle { float x, y, speedX, speedY, size; uint16_t color; bool active; };
enum UIEffect { EFFECT_NONE, EFFECT_SNOW, EFFECT_RAIN, EFFECT_CONFETTI, EFFECT_SAKURA, EFFECT_CLOUDS, EFFECT_SUN };

class UIManager {
public:
    UIManager();
    void begin(DisplayDriver* display, CoreManager* core, NetworkManager* net, SettingsManager* settings, UsbManager* usb);
    void update(); 
    void draw();   
    void setScene(UIScene scene);
    UIScene getScene(); 
    
    // Interactions
    bool handleMenuClick(int touchX, int touchY);
    int handleDeckClick(int touchX, int touchY); 
    TamaAction handleTamagotchiClick(int touchX, int touchY);
    
    // Notifications & Effets
    void showNotification(String message, uint32_t color, int durationMs);
    void setEffect(UIEffect effect);
    void setWeatherEffect(UIEffect effect); 

    // Navigation
    void nextDeckPage();
    void prevDeckPage();
    int getDeckPage() { return _currentDeckPage; }

private:
    // Managers
    DisplayDriver* _display;
    CoreManager* _core;
    NetworkManager* _net;
    SettingsManager* _settings; 
    UsbManager* _usb; 
    
    // Renderers (Sous-traitants)
    FaceRenderer      _face; 
    CountdownRenderer _countdown; 
    DeckRenderer      _deck; 
    MenuRenderer      _menu;
    OverlayRenderer   _overlay; // ⬅️ Gère le HUD et les Notifs

    LGFX_Sprite _mainSprite; 
    UIScene _currentScene = SCENE_BOOT;
    
    void _initMenu();

    int _currentDeckPage = 0;
    unsigned long _sceneStartTime = 0; const unsigned long SCENE_TIMEOUT = 10000; 

    // Effets Particules
    UIEffect _currentEffect = EFFECT_NONE;
    UIEffect _weatherEffect = EFFECT_NONE; 
    std::vector<Particle> _particles;
    const int MAX_PARTICLES = 60; 
    void _initParticles(UIEffect type);
    void _updateParticles();
    void _drawParticles();
    
    // Dessin Scènes restantes (non migrées)
    void _drawSetupWifi(); 
    void _drawTamaMenu(); 
    void _drawMouseStatus(); 
    void _drawAppTrackpad(); 
};
#endif