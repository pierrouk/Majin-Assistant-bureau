#ifndef UI_MANAGER_H
#define UI_MANAGER_H

#include <Arduino.h>
#include <LovyanGFX.hpp>
#include "../hal/DisplayDriver.h"
#include "CoreManager.h"
#include "NetworkManager.h"
#include "SettingsManager.h" 
#include "UsbManager.h" 
#include "FaceRenderer.h" 
#include "CountdownRenderer.h" 

enum UIScene { SCENE_BOOT, SCENE_FACE, SCENE_MENU, SCENE_APP_TRACKPAD, SCENE_APP_STREAMDECK, SCENE_SETTINGS, SCENE_SETUP_WIFI, SCENE_COUNTDOWN, SCENE_TAMA_MENU };
enum UIEffect { EFFECT_NONE, EFFECT_SNOW, EFFECT_RAIN, EFFECT_CONFETTI, EFFECT_SAKURA, EFFECT_CLOUDS, EFFECT_SUN };
struct Particle { float x, y, speedX, speedY, size; uint16_t color; bool active; };
struct AppIcon { String name; uint32_t color; int x, y, radius; UIScene targetScene; };
enum TamaAction { ACTION_NONE, ACTION_FEED, ACTION_PLAY, ACTION_SLEEP, ACTION_WAKE };

class UIManager {
public:
    UIManager();
    void begin(DisplayDriver* display, CoreManager* core, NetworkManager* net, SettingsManager* settings, UsbManager* usb);
    void update(); 
    void draw();   
    void setScene(UIScene scene);
    UIScene getScene(); 
    // 👇 AJOUTE CETTE LIGNE ICI
    int handleDeckClick(int touchX, int touchY);
    bool handleMenuClick(int touchX, int touchY);
    TamaAction handleTamagotchiClick(int touchX, int touchY);
    void showNotification(String message, uint32_t color, int durationMs);
    void setEffect(UIEffect effect);
    void setWeatherEffect(UIEffect effect); 

    void nextDeckPage();
    void prevDeckPage();
    int getDeckPage() { return _currentDeckPage; }

private:
    DisplayDriver* _display;
    CoreManager* _core;
    NetworkManager* _net;
    SettingsManager* _settings; 
    UsbManager* _usb; 
    // 🕸️ MENU HEXAGONAL (Variables)
    float _menuScrollCurrent = 0.0; // Position actuelle (animation fluide)
    int   _menuScrollTarget = 0;    // Index de l'app ciblée (0, 1, 2...)
    FaceRenderer _face; 
    CountdownRenderer _countdown; 

    LGFX_Sprite _mainSprite; 
    UIScene _currentScene = SCENE_BOOT;
    std::vector<AppIcon> _apps;
    void _initApps();           

    int _currentDeckPage = 0;
    unsigned long _sceneStartTime = 0; const unsigned long SCENE_TIMEOUT = 10000; 
    String _notifMsg = ""; uint32_t _notifColor = 0; unsigned long _notifStartTime = 0; int _notifDuration = 0;

    UIEffect _currentEffect = EFFECT_NONE;
    UIEffect _weatherEffect = EFFECT_NONE; 
    std::vector<Particle> _particles;
    const int MAX_PARTICLES = 60; 
    void _initParticles(UIEffect type);
    void _updateParticles();
    void _drawParticles(); // Renommé pour clarté
    
    // 🎨 OVERLAY UNIFIÉ (HUD)
    void _drawUnifiedOverlay(); // Remplace Widgets + Overlay Météo
    void _drawStatusBar();      // Barre du haut
    void _drawBottomWidgets();  // Météo + Countdown

    // Helpers Dessin
    void _drawWiFiIcon(int x, int y, bool connected);
    void _drawWeatherWidget(int x, int y);
    void _drawCountdownWidget(int x, int y);
    

    
    // 🆕 Helper Hexagone
    void _drawHexagon(LGFX_Sprite* spr, int x, int y, int radius, uint32_t color, bool filled);
    // Dessin Scènes
    void _drawSceneMenu();
    void _drawAppTrackpad();   
    void _drawAppStreamDeck(); 
    void _drawSetupWifi(); 
    void _drawNotification(); 
    void _drawTamaMenu(); 
    void _drawMouseStatus(); 
};
#endif