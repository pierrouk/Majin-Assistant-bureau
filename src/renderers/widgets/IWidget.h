#ifndef IWIDGET_H
#define IWIDGET_H

#include <Arduino.h>
#include <LovyanGFX.hpp>
#include "../../managers/CoreManager.h"
#include "../../managers/NetworkManager.h"
#include "../../managers/SettingsManager.h"

class IWidget {
public:
    virtual ~IWidget() {}
    virtual void begin(CoreManager* core, NetworkManager* net, SettingsManager* settings) = 0;
    virtual void update() = 0;
    virtual void draw(LGFX_Sprite* spr, int x, int y) = 0;
};

#endif