#ifndef WIFI_WIDGET_H
#define WIFI_WIDGET_H

#include "IWidget.h"
#include "../../../include/Theme.h"

class WiFiWidget : public IWidget {
public:
    void begin(CoreManager* core, NetworkManager* net, SettingsManager* settings) override;
    void update() override;
    void draw(LGFX_Sprite* spr, int x, int y) override;

private:
    NetworkManager* _net;
};

#endif