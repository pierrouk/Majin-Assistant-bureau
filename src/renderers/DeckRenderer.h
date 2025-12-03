#ifndef DECK_RENDERER_H
#define DECK_RENDERER_H

#include <Arduino.h>
#include <LovyanGFX.hpp>
#include "../managers/SettingsManager.h"

class DeckRenderer {
public:
    DeckRenderer();
    void begin(SettingsManager* settings);
    
    void draw(LGFX_Sprite* sprite, int pageIndex);
    int handleClick(int touchX, int touchY, int pageIndex);

private:
    SettingsManager* _settings;
    
    void _drawHexagon(LGFX_Sprite* spr, int x, int y, int radius, uint32_t color, bool filled);
    void _drawIcon(LGFX_Sprite* spr, int id, int x, int y, uint16_t color);
};

#endif