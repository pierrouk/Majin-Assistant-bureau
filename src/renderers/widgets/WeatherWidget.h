#ifndef WEATHER_WIDGET_H
#define WEATHER_WIDGET_H

#include "IWidget.h"
#include "../../../include/Theme.h"

class WeatherWidget : public IWidget {
public:
    void begin(CoreManager* core, NetworkManager* net, SettingsManager* settings) override;
    void update() override;
    void draw(LGFX_Sprite* spr, int x, int y) override;

private:
    CoreManager* _core;
    SettingsManager* _settings;
};

#endif