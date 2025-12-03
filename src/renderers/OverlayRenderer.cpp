#include "OverlayRenderer.h"
#include "../../include/Theme.h"
#include <time.h>

OverlayRenderer::OverlayRenderer() {}

OverlayRenderer::~OverlayRenderer() {
    for (auto widget : _widgets) { delete widget; }
    _widgets.clear();
}

void OverlayRenderer::begin(CoreManager* core, NetworkManager* net, SettingsManager* settings) {
    _core = core; _net = net; _settings = settings;

    // Ordre important des widgets dans le vecteur : 0=WiFi, 1=Météo, 2=Countdown
    _widgets.push_back(new WiFiWidget());
    _widgets.push_back(new WeatherWidget());
    _widgets.push_back(new CountdownWidget());

    for (auto widget : _widgets) {
        widget->begin(core, net, settings);
    }
}

void OverlayRenderer::showNotification(String message, uint32_t color, int durationMs) {
    _notifMsg = message; _notifColor = color; _notifDuration = durationMs; _notifStartTime = millis();
}

void OverlayRenderer::update() {
    if (_notifDuration > 0 && (millis() - _notifStartTime > _notifDuration)) {
        _notifMsg = ""; _notifDuration = 0;
    }
    for (auto widget : _widgets) { widget->update(); }
}

void OverlayRenderer::draw(LGFX_Sprite* spr) {
    if (_settings->isSetupDone()) {
        _drawStatusBar(spr);
        _drawBottomWidgets(spr);
    }
    if (_notifMsg != "") {
        _drawNotificationBubble(spr);
    }
}

void OverlayRenderer::_drawStatusBar(LGFX_Sprite* spr) {
    int y = 5;
    if (_settings->getShowSensors()) {
        spr->setFont(FONT_SMALL); spr->setTextDatum(top_left); spr->setTextColor(COLOR_PRIMARY);
        String tempStr = String(_core->getTemp(), 1) + "C"; spr->drawString(tempStr, 5, y);
    }
    if (_settings->getShowTime()) {
        spr->setFont(FONT_UI); spr->setTextColor(COLOR_WHITE); spr->setTextDatum(top_center);
        String timeStr = _net->isConnected() ? _core->getTimeString().substring(0, 5) : "--:--";
        spr->drawString(timeStr, 160, y);
    }
    // Widget WiFi (Index 0)
    if (_widgets.size() > 0) _widgets[0]->draw(spr, 305, y + 4);
}

void OverlayRenderer::_drawBottomWidgets(LGFX_Sprite* spr) {
    // Widget Météo (Index 1) - 10, 200
    if (_widgets.size() > 1) _widgets[1]->draw(spr, 10, 200);
    
    // Widget Countdown (Index 2) - 220, 205
    if (_widgets.size() > 2) _widgets[2]->draw(spr, 220, 205);
}

void OverlayRenderer::_drawNotificationBubble(LGFX_Sprite* spr) {
    int x = 220; int y = 60; int w = 140; int h = 50; int r = 10;
    int baseX1 = x - 40; int baseX2 = x - 10; int baseY  = y + 15; int tipX = 160; int tipY = 100;
    spr->fillTriangle(baseX1, baseY, baseX2, baseY, tipX, tipY, COLOR_WHITE);
    spr->fillRoundRect(x - w/2, y - h/2, w, h, r, COLOR_WHITE);
    spr->drawRoundRect(x - w/2, y - h/2, w, h, r, _notifColor);
    spr->setFont(FONT_UI); spr->setTextDatum(middle_center); 
    uint16_t textColor = (_notifColor == COLOR_WHITE) ? COLOR_BLACK : _notifColor;
    if (_notifColor == COLOR_PRIMARY) textColor = COLOR_BLACK;
    spr->setTextColor(textColor); 
    if (spr->textWidth(_notifMsg) > (w - 10)) { spr->setFont(FONT_SMALL); }
    spr->drawString(_notifMsg, x, y); 
}