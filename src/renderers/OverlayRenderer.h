#ifndef OVERLAY_RENDERER_H
#define OVERLAY_RENDERER_H

#include <Arduino.h>
#include <LovyanGFX.hpp>
#include <vector>
#include "../managers/CoreManager.h"
#include "../managers/NetworkManager.h"
#include "../managers/SettingsManager.h"

// Widgets
#include "widgets/IWidget.h"
#include "widgets/WiFiWidget.h"
#include "widgets/WeatherWidget.h"
#include "widgets/CountdownWidget.h"

class OverlayRenderer {
public:
    OverlayRenderer();
    ~OverlayRenderer();

    void begin(CoreManager* core, NetworkManager* net, SettingsManager* settings);
    void update();
    void draw(LGFX_Sprite* sprite);
    
    void showNotification(String message, uint32_t color, int durationMs);

private:
    CoreManager* _core;
    NetworkManager* _net;
    SettingsManager* _settings;

    std::vector<IWidget*> _widgets;

    String _notifMsg = "";
    uint32_t _notifColor = 0;
    unsigned long _notifStartTime = 0;
    int _notifDuration = 0;

    void _drawStatusBar(LGFX_Sprite* spr);
    void _drawBottomWidgets(LGFX_Sprite* spr);
    void _drawNotificationBubble(LGFX_Sprite* spr);
};

#endif