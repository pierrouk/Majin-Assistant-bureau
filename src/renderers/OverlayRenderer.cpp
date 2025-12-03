#include "OverlayRenderer.h"
#include "../../include/Theme.h"
#include <time.h>

OverlayRenderer::OverlayRenderer() {}

void OverlayRenderer::begin(CoreManager* core, NetworkManager* net, SettingsManager* settings) {
    _core = core;
    _net = net;
    _settings = settings;
}

void OverlayRenderer::showNotification(String message, uint32_t color, int durationMs) {
    _notifMsg = message;
    _notifColor = color;
    _notifDuration = durationMs;
    _notifStartTime = millis();
}

void OverlayRenderer::update() {
    if (_notifDuration > 0 && (millis() - _notifStartTime > _notifDuration)) {
        _notifMsg = "";
        _notifDuration = 0;
    }
}

void OverlayRenderer::draw(LGFX_Sprite* spr) {
    // Si setup pas fait, on n'affiche rien (sauf notif éventuellement)
    if (_settings->isSetupDone()) {
        _drawStatusBar(spr);
        _drawBottomWidgets(spr);
    }
    
    // Notification (Priorité Max)
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
    _drawWiFiIcon(spr, 305, y + 4, _net->isConnected());
}

void OverlayRenderer::_drawWiFiIcon(LGFX_Sprite* spr, int x, int y, bool connected) {
    uint16_t color = connected ? COLOR_WHITE : 0x5555;
    spr->drawArc(x, y+6, 8, 6, 225, 315, color);
    spr->drawArc(x, y+6, 5, 4, 225, 315, color);
    spr->fillCircle(x, y+6, 1, color);
    if (!connected) { spr->drawLine(x-6, y-2, x+6, y+8, COLOR_DANGER); }
}

void OverlayRenderer::_drawBottomWidgets(LGFX_Sprite* spr) {
    if (_settings->getWeatherEnabled()) { _drawWeatherWidget(spr, 10, 200); }
    if (_net->isConnected()) {
        String evtName = _settings->getEventName();
        if (evtName.length() > 0) { _drawCountdownWidget(spr, 220, 205); }
    }
}

void OverlayRenderer::_drawCountdownWidget(LGFX_Sprite* spr, int x, int y) {
    String evtName = _settings->getEventName();
    int type = _settings->getEventType();
    time_t now; time(&now);
    unsigned long target = _settings->getEventTimestamp();
    long diff = target - (unsigned long)now;
    if (diff < 0) diff = 0;
    long d = diff / 86400;

    _drawEventIcon(spr, type, x + 8, y + 10, COLOR_WHITE);

    int textX = x + 25;
    spr->setTextDatum(top_left);
    spr->setFont(FONT_SMALL); spr->setTextColor(0xCE79);
    if(evtName.length() > 10) evtName = evtName.substring(0, 10) + ".";
    spr->drawString(evtName, textX, y + 2);
    spr->setFont(FONT_UI); spr->setTextColor(COLOR_WHITE);
    spr->drawString("J-" + String(d), textX, y + 12);
}

void OverlayRenderer::_drawWeatherWidget(LGFX_Sprite* spr, int x, int y) {
    int code = _core->getExternalWeatherCode();
    float temp = _core->getExternalTemp();
    String city = _settings->getCityName();
    if (code == -1) return;

    spr->setTextDatum(top_left);
    int ix = x + 20; int iy = y + 15;
    uint16_t sunCol = 0xFFE0; uint16_t cloudCol = 0xCE79; uint16_t rainCol = 0x001F;

    if (code == 0) { 
        spr->fillCircle(ix, iy, 7, sunCol); 
        for(int i=0; i<8; i++) { float a = i * (PI/4); spr->drawLine(ix + cos(a)*9, iy + sin(a)*9, ix + cos(a)*12, iy + sin(a)*12, sunCol); }
    } else if (code <= 3) { 
        spr->fillCircle(ix+4, iy-4, 5, sunCol); spr->fillCircle(ix-3, iy+2, 6, COLOR_WHITE); spr->fillCircle(ix+5, iy+3, 5, COLOR_WHITE); 
    } else if (code <= 48) { 
        spr->fillCircle(ix-4, iy+2, 6, cloudCol); spr->fillCircle(ix+4, iy, 7, cloudCol); spr->fillCircle(ix+2, iy+5, 6, cloudCol);
    } else if ((code >= 51 && code <= 67) || (code >= 80 && code <= 82)) { 
        spr->fillCircle(ix-4, iy, 6, cloudCol); spr->fillCircle(ix+4, iy-2, 7, cloudCol); 
        spr->drawLine(ix-2, iy+8, ix-4, iy+13, rainCol); spr->drawLine(ix+2, iy+8, ix, iy+13, rainCol); spr->drawLine(ix+6, iy+8, ix+4, iy+13, rainCol);
    } else if (code >= 71 && code <= 86) { 
        spr->fillCircle(ix, iy, 8, cloudCol); spr->fillCircle(ix-3, iy+10, 2, COLOR_WHITE); spr->fillCircle(ix+3, iy+8, 2, COLOR_WHITE);
    } else if (code >= 95) { 
        spr->fillCircle(ix, iy, 8, 0x3333); spr->fillTriangle(ix-2, iy+8, ix+2, iy+8, ix-4, iy+16, sunCol);
    }

    int textX = x + 45; 
    spr->setFont(FONT_UI); spr->setTextColor(COLOR_WHITE); spr->setTextDatum(bottom_left); spr->drawString(String(temp, 1) + "C", textX, y + 18);
    spr->setFont(FONT_SMALL); spr->setTextColor(0xCE79); spr->setTextDatum(top_left); 
    if(city.length() > 9) city = city.substring(0,9); spr->drawString(city, textX, y + 20);
}

void OverlayRenderer::_drawEventIcon(LGFX_Sprite* spr, int type, int x, int y, uint16_t color) {
    switch (type) {
        case 1: spr->fillRect(x-6, y, 12, 6, 0xFCA0); spr->fillRect(x-6, y-3, 12, 3, 0xF80A); spr->drawLine(x, y-3, x, y-7, 0xFFE0); spr->drawPixel(x, y-8, 0xF800); break;
        case 2: spr->fillTriangle(x, y-8, x-6, y+4, x+6, y+4, 0x07E0); spr->fillRect(x-1, y+4, 2, 2, 0x9280); spr->drawPixel(x, y-8, 0xFFE0); break;
        case 3: spr->fillCircle(x, y, 6, 0xFFE0); spr->drawCircle(x, y, 6, 0xFCA0); spr->drawLine(x, y-8, x, y+8, 0xFFE0); spr->drawLine(x-8, y, x+8, y, 0xFFE0); break;
        case 4: spr->drawCircle(x, y+2, 5, 0xFFE0); spr->fillCircle(x, y-4, 3, 0x07FF); spr->drawPixel(x-1, y-5, COLOR_WHITE); break;
        case 5: spr->fillTriangle(x-8, y+6, x-2, y-4, x+4, y+6, 0x7BEF); spr->fillTriangle(x, y+6, x+4, y, x+8, y+6, 0x3333); spr->fillTriangle(x-2, y-4, x-3, y-2, x-1, y-2, COLOR_WHITE); break;
        default: spr->drawCircle(x, y, 6, color); spr->drawLine(x, y, x, y-4, color); spr->drawLine(x, y, x+3, y, color); break;
    }
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