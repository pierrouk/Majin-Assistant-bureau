#include "MenuRenderer.h"
#include "../../include/Theme.h"

#ifndef DEG_TO_RAD
#define DEG_TO_RAD 0.017453292519943295769236907684886
#endif

MenuRenderer::MenuRenderer() {}

void MenuRenderer::addApp(String name, uint32_t color, UIScene target) {
    // x,y,radius ne sont pas stockés ici, ils sont calculés à la volée
    _apps.push_back({name, color, 0, 0, 0, target});
}

void MenuRenderer::update() {
    // Animation fluide (Lerp)
    _scrollCurrent += (_scrollTarget - _scrollCurrent) * 0.2;
}

void MenuRenderer::draw(LGFX_Sprite* spr) {
    spr->fillScreen(COLOR_BG); 
    
    // Titre
    spr->setTextDatum(top_center); 
    spr->setTextColor(COLOR_WHITE); 
    spr->setFont(FONT_TITLE); 
    spr->drawString("APPS", 160, 20); 

    int centerX = 160; 
    int centerY = 120; 
    int spacing = 100; 
    int baseRadius = 38; 

    for (int i = 0; i < _apps.size(); i++) {
        float offset = (i - _scrollCurrent);
        int x = centerX + (offset * spacing);
        
        // Effet Vague
        int y = centerY + (sin(offset) * 10);

        // Optimisation : On ne dessine que ce qui est visible
        if (x > -50 && x < 370) {
            float dist = abs(offset);
            
            // Zoom et Luminosité
            float scale = (dist < 0.5) ? 1.0 : 0.9; 
            int radius = baseRadius * scale;
            uint32_t color = _apps[i].color;
            if (dist > 0.8) { color = (color & 0xF7DE) >> 1; }

            // Hexagone Fond
            _drawHexagon(spr, x, y, radius, color, true);
            
            // Contour
            uint32_t outline = (dist < 0.4) ? COLOR_WHITE : color;
            _drawHexagon(spr, x, y, radius + 2, outline, false);
            if(dist < 0.4) _drawHexagon(spr, x, y, radius + 3, outline, false); 

            // Texte
            if (dist < 1.2) {
                spr->setFont(FONT_SMALL); 
                spr->setTextColor(COLOR_WHITE); 
                spr->setTextDatum(middle_center); 
                spr->drawString(_apps[i].name, x, y);
            }
        }
    }
    
    // Pagination (Points)
    int dotY = 220; 
    int totalW = _apps.size() * 15; 
    int startDotX = 160 - (totalW / 2);
    for (int i = 0; i < _apps.size(); i++) {
        uint16_t c = (i == _scrollTarget) ? COLOR_PRIMARY : 0x3333;
        spr->fillCircle(startDotX + (i * 15), dotY, 3, c);
    }
}

UIScene MenuRenderer::handleClick(int touchX, int touchY) {
    // Zone Centrale (Validation)
    if (touchX > 110 && touchX < 210 && touchY > 80 && touchY < 160) {
        // On retourne la scène de l'app sélectionnée
        return _apps[_scrollTarget].targetScene;
    }
    
    // Zone Gauche (Scroll Arrière)
    if (touchX < 100) { 
        if (_scrollTarget > 0) _scrollTarget--; 
    }
    
    // Zone Droite (Scroll Avant)
    else if (touchX > 220) { 
        if (_scrollTarget < _apps.size() - 1) _scrollTarget++; 
    }
    
    // Si on a juste scrollé, on reste sur le menu
    return SCENE_MENU;
}
String MenuRenderer::getSelectedAppName() {
    if (_apps.empty()) return "";
    // Sécurité bornes
    if (_scrollTarget < 0 || _scrollTarget >= _apps.size()) return "";
    return _apps[_scrollTarget].name;
}
void MenuRenderer::_drawHexagon(LGFX_Sprite* spr, int x, int y, int radius, uint32_t color, bool filled) {
    int xs[6], ys[6];
    for (int i = 0; i < 6; i++) {
        float angle = (60 * i - 30) * DEG_TO_RAD;
        xs[i] = x + cos(angle) * radius; 
        ys[i] = y + sin(angle) * radius;
    }
    if (filled) {
        for (int i = 0; i < 6; i++) { spr->fillTriangle(x, y, xs[i], ys[i], xs[(i+1)%6], ys[(i+1)%6], color); }
    } else {
        for (int i = 0; i < 6; i++) { spr->drawLine(xs[i], ys[i], xs[(i+1)%6], ys[(i+1)%6], color); }
    }
}