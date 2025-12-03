#include "WiFiWidget.h"

void WiFiWidget::begin(CoreManager* core, NetworkManager* net, SettingsManager* settings) {
    _net = net;
}

void WiFiWidget::update() {
    // Rien de spécial à mettre à jour
}

void WiFiWidget::draw(LGFX_Sprite* spr, int x, int y) {
    bool connected = _net->isConnected();
    uint16_t color = connected ? COLOR_WHITE : 0x5555; // Blanc ou Gris
    
    // Dessin de l'Arc WiFi
    spr->drawArc(x, y+6, 8, 6, 225, 315, color);
    spr->drawArc(x, y+6, 5, 4, 225, 315, color);
    spr->fillCircle(x, y+6, 1, color);

    // Croix rouge si déconnecté
    if (!connected) {
        spr->drawLine(x-6, y-2, x+6, y+8, COLOR_DANGER);
    }
}