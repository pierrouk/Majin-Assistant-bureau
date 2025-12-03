#ifndef OVERLAY_RENDERER_H
#define OVERLAY_RENDERER_H

#include <Arduino.h>
#include <LovyanGFX.hpp>
#include "../managers/CoreManager.h"
#include "../managers/NetworkManager.h"
#include "../managers/SettingsManager.h"

class OverlayRenderer {
public:
    OverlayRenderer();
    void begin(CoreManager* core, NetworkManager* net, SettingsManager* settings);
    
    // Dessin principal
    void draw(LGFX_Sprite* sprite);
    
    // Notification
    void showNotification(String message, uint32_t color, int durationMs);
    void update(); // Pour gérer le timeout de la notif

private:
    CoreManager* _core;
    NetworkManager* _net;
    SettingsManager* _settings;

    // État Notification
    String _notifMsg = "";
    uint32_t _notifColor = 0;
    unsigned long _notifStartTime = 0;
    int _notifDuration = 0;

    // Helpers internes (Privés)
    void _drawStatusBar(LGFX_Sprite* spr);
    void _drawBottomWidgets(LGFX_Sprite* spr);
    void _drawWiFiIcon(LGFX_Sprite* spr, int x, int y, bool connected);
    void _drawWeatherWidget(LGFX_Sprite* spr, int x, int y);
    void _drawCountdownWidget(LGFX_Sprite* spr, int x, int y);
    void _drawEventIcon(LGFX_Sprite* spr, int type, int x, int y, uint16_t color);
    void _drawNotificationBubble(LGFX_Sprite* spr);
};

#endif