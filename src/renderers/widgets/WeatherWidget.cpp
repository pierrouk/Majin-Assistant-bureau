#include "WeatherWidget.h"

void WeatherWidget::begin(CoreManager* core, NetworkManager* net, SettingsManager* settings) {
    _core = core;
    _settings = settings;
}

void WeatherWidget::update() {
    // Pas d'animation interne
}

void WeatherWidget::draw(LGFX_Sprite* spr, int x, int y) {
    // Si désactivé globalement, on sort
    if (!_settings->getWeatherEnabled()) return;

    int code = _core->getExternalWeatherCode();
    float temp = _core->getExternalTemp();
    String city = _settings->getCityName();

    spr->setTextDatum(top_left);
    int ix = x + 20; 
    int iy = y + 15;
    int textX = x + 45;

    // CAS 1 : EN ATTENTE DE DONNÉES (-1)
    if (code == -1) {
        spr->drawCircle(ix, iy, 8, 0x5555); // Cercle gris
        spr->setTextColor(0x5555);
        spr->setFont(FONT_SMALL);
        spr->drawString("?", ix - 3, iy - 4);
        
        spr->setFont(FONT_UI);
        spr->setTextColor(0x5555);
        spr->drawString("...", textX, y + 18);
        return;
    }

    // CAS 2 : DONNÉES REÇUES
    uint16_t sunCol = 0xFFE0; 
    uint16_t cloudCol = 0xCE79; 
    uint16_t rainCol = 0x001F;

    if (code == 0) { 
        spr->fillCircle(ix, iy, 7, sunCol); 
        for(int i=0; i<8; i++) { float a = i * (PI/4); spr->drawLine(ix + cos(a)*9, iy + sin(a)*9, ix + cos(a)*12, iy + sin(a)*12, sunCol); }
    } 
    else if (code <= 3) { 
        spr->fillCircle(ix+4, iy-4, 5, sunCol); spr->fillCircle(ix-3, iy+2, 6, COLOR_WHITE); spr->fillCircle(ix+5, iy+3, 5, COLOR_WHITE); 
    } 
    else if (code <= 48) { 
        spr->fillCircle(ix-4, iy+2, 6, cloudCol); spr->fillCircle(ix+4, iy, 7, cloudCol); spr->fillCircle(ix+2, iy+5, 6, cloudCol);
    } 
    else if ((code >= 51 && code <= 67) || (code >= 80 && code <= 82)) { 
        spr->fillCircle(ix-4, iy, 6, cloudCol); spr->fillCircle(ix+4, iy-2, 7, cloudCol); 
        spr->drawLine(ix-2, iy+8, ix-4, iy+13, rainCol); spr->drawLine(ix+2, iy+8, ix, iy+13, rainCol); spr->drawLine(ix+6, iy+8, ix+4, iy+13, rainCol);
    } 
    else if (code >= 71 && code <= 86) { 
        spr->fillCircle(ix, iy, 8, cloudCol); spr->fillCircle(ix-3, iy+10, 2, COLOR_WHITE); spr->fillCircle(ix+3, iy+8, 2, COLOR_WHITE);
    } 
    else if (code >= 95) { 
        spr->fillCircle(ix, iy, 8, 0x3333); spr->fillTriangle(ix-2, iy+8, ix+2, iy+8, ix-4, iy+16, sunCol);
    }

    spr->setFont(FONT_UI); spr->setTextColor(COLOR_WHITE); spr->setTextDatum(bottom_left); 
    spr->drawString(String(temp, 1) + "C", textX, y + 18);
    
    spr->setFont(FONT_SMALL); spr->setTextColor(0xCE79); spr->setTextDatum(top_left); 
    if(city.length() > 9) city = city.substring(0,9);
    spr->drawString(city, textX, y + 20);
}