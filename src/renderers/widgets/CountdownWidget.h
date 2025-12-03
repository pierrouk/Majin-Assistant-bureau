#ifndef COUNTDOWN_WIDGET_H
#define COUNTDOWN_WIDGET_H

#include "IWidget.h"
#include "../../../include/Theme.h"
#include <time.h> 

class CountdownWidget : public IWidget {
public:
    void begin(CoreManager* core, NetworkManager* net, SettingsManager* settings) override;
    void update() override;
    void draw(LGFX_Sprite* spr, int x, int y) override;

private:
    SettingsManager* _settings;
    NetworkManager* _net;

    // Helper interne pour dessiner l'icône spécifique
    void _drawEventIcon(LGFX_Sprite* spr, int type, int x, int y, uint16_t color);
};

#endif