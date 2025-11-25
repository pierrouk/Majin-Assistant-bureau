#ifndef COUNTDOWN_RENDERER_H
#define COUNTDOWN_RENDERER_H

#include <Arduino.h>
#include <LovyanGFX.hpp>
#include "CoreManager.h"
#include "SettingsManager.h"
#include "../../include/Theme.h"

class CountdownRenderer {
public:
    CountdownRenderer();
    void begin(CoreManager* core, SettingsManager* settings);
    
    // Dessine l'écran complet
    void draw(LGFX_Sprite* sprite);

private:
    CoreManager* _core;
    SettingsManager* _settings;

    // Helpers de dessin d'icônes
    void _drawIconGeneric(LGFX_Sprite* spr, int x, int y);
    void _drawIconBirthday(LGFX_Sprite* spr, int x, int y);
    void _drawIconXmas(LGFX_Sprite* spr, int x, int y);
    void _drawIconVacation(LGFX_Sprite* spr, int x, int y);
    void _drawIconWedding(LGFX_Sprite* spr, int x, int y); // Bague
    void _drawIconMountain(LGFX_Sprite* spr, int x, int y); // Montagne
};

#endif