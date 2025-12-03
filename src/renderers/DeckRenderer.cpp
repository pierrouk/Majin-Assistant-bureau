#include "DeckRenderer.h"
#include "../../include/Theme.h"

#ifndef DEG_TO_RAD
#define DEG_TO_RAD 0.017453292519943295769236907684886
#endif

DeckRenderer::DeckRenderer() {}

void DeckRenderer::begin(SettingsManager* settings) {
    _settings = settings;
}

void DeckRenderer::draw(LGFX_Sprite* sprite, int pageIndex) {
    sprite->fillScreen(TFT_BLACK); 
    
    // Titre
    sprite->setTextColor(COLOR_WHITE); 
    sprite->setFont(FONT_UI); 
    sprite->setTextDatum(top_center); 
    sprite->drawString("DECK PAGE " + String(pageIndex + 1) + "/4", 160, 15);

    // Grille Hexagonale
    int radius = 38;
    int startX = 75; int startY = 85;
    int gapX = 85;   int gapY = 75;
    
    int startIndex = pageIndex * 6; 

    for(int i=0; i<6; i++) { 
        int row = i / 3; int col = i % 3;
        int cx = startX + col * gapX;
        int cy = startY + row * gapY;
        if (row == 1) cx += gapX / 2; 

        DeckButton btn = _settings->getDeckButton(startIndex + i); 

        if (btn.active) { 
            _drawHexagon(sprite, cx, cy, radius, btn.color, true);
            if (btn.iconID > 0) { 
                _drawIcon(sprite, btn.iconID, cx, cy - 5, COLOR_WHITE); 
                sprite->setTextDatum(bottom_center); sprite->setTextColor(COLOR_WHITE); 
                sprite->setFont(FONT_SMALL); sprite->drawString(btn.label, cx, cy + 15); 
            } else { 
                sprite->setTextDatum(middle_center); sprite->setTextColor(COLOR_WHITE); 
                sprite->setFont(FONT_UI); sprite->drawString(btn.label, cx, cy); 
            } 
        } else { 
            _drawHexagon(sprite, cx, cy, radius, 0x2945, true); 
            sprite->setTextColor(0x5555); sprite->setTextDatum(middle_center); 
            sprite->setFont(FONT_UI); sprite->drawString("+", cx, cy); 
        } 
    } 

    sprite->setFont(FONT_SMALL); sprite->setTextColor(0x5555); 
    sprite->setTextDatum(bottom_center); sprite->drawString("< Swipe >", 160, 235); 
}

int DeckRenderer::handleClick(int touchX, int touchY, int pageIndex) {
    int radius = 38;
    int startX = 75; int startY = 85;
    int gapX = 85;   int gapY = 75;
    long detectRadiusSq = (radius - 5) * (radius - 5); 

    for(int i=0; i<6; i++) {
        int row = i / 3; int col = i % 3;
        int cx = startX + col * gapX;
        int cy = startY + row * gapY;
        if (row == 1) cx += gapX / 2; 

        long dx = touchX - cx;
        long dy = touchY - cy;
        long distSq = dx*dx + dy*dy;

        if (distSq < detectRadiusSq) {
            int globalIndex = (pageIndex * 6) + i;
            DeckButton btn = _settings->getDeckButton(globalIndex);
            if (btn.active) return globalIndex;
        }
    }
    return -1;
}

void DeckRenderer::_drawHexagon(LGFX_Sprite* spr, int x, int y, int radius, uint32_t color, bool filled) {
    int xs[6], ys[6];
    for (int i = 0; i < 6; i++) {
        float angle = (60 * i - 30) * DEG_TO_RAD;
        xs[i] = x + cos(angle) * radius; ys[i] = y + sin(angle) * radius;
    }
    if (filled) {
        for (int i = 0; i < 6; i++) { spr->fillTriangle(x, y, xs[i], ys[i], xs[(i+1)%6], ys[(i+1)%6], color); }
    } else {
        for (int i = 0; i < 6; i++) { spr->drawLine(xs[i], ys[i], xs[(i+1)%6], ys[(i+1)%6], color); }
    }
}

void DeckRenderer::_drawIcon(LGFX_Sprite* spr, int id, int x, int y, uint16_t color) {
    spr->setTextColor(color);
    switch (id) {
        case 1: spr->fillTriangle(x-5, y-8, x-5, y+8, x+8, y, color); break; 
        case 2: spr->fillTriangle(x-8, y, x-2, y-6, x-2, y+6, color); spr->fillRect(x-8, y-2, 4, 4, color); spr->drawLine(x, y-6, x+8, y+6, color); spr->drawLine(x, y+6, x+8, y-6, color); break; 
        case 3: spr->fillRoundRect(x-10, y-6, 16, 12, 2, color); spr->fillTriangle(x+6, y-3, x+6, y+3, x+10, y-5, color); break; 
        case 4: spr->fillRoundRect(x-3, y-8, 6, 12, 3, color); spr->drawArc(x, y-2, 8, 6, 45, 315, color); spr->drawLine(x, y+6, x, y+10, color); break; 
        case 5: spr->fillCircle(x, y, 8, color); break; 
        case 6: spr->fillRect(x-6, y-8, 12, 16, color); spr->fillTriangle(x+6, y-8, x+6, y-2, x, y-8, 0x0000); break; 
        case 7: spr->fillRect(x-8, y-8, 16, 16, color); spr->fillRect(x-4, y-8, 8, 6, 0x0000); break; 
        case 8: spr->fillTriangle(x, y-10, x-10, y, x+10, y, color); spr->fillRect(x-6, y, 12, 8, color); break; 
        case 9: spr->fillRoundRect(x-10, y-8, 20, 14, 4, color); spr->fillTriangle(x-5, y+6, x+5, y+6, x-2, y+10, color); break; 
        case 10: spr->fillRoundRect(x-9, y-7, 18, 14, 4, color); spr->fillCircle(x-4, y-1, 2, 0x0000); spr->fillCircle(x+4, y-1, 2, 0x0000); break;
        case 11: spr->drawCircle(x, y, 9, color); spr->drawCircle(x, y, 3, color); spr->drawLine(x, y-3, x, y-9, color); spr->drawLine(x-2, y+2, x-7, y+6, color); spr->drawLine(x+2, y+2, x+7, y+6, color); break;
        case 12: spr->drawCircle(x, y, 9, color); spr->fillCircle(x, y, 3, color); spr->drawLine(x, y, x+9, y, color); spr->drawLine(x, y, x-5, y-8, color); spr->drawLine(x, y, x-5, y+8, color); break;
        case 13: spr->fillRect(x-9, y-6, 18, 12, color); spr->fillRect(x-9, y-9, 8, 3, color); break;
        case 14: spr->fillRoundRect(x-3, y-8, 6, 12, 3, color); spr->drawArc(x, y-2, 8, 6, 45, 315, color); spr->drawLine(x-8, y-8, x+8, y+8, COLOR_DANGER); break;
        case 15: spr->fillRoundRect(x-10, y-6, 16, 12, 2, color); spr->fillTriangle(x+6, y-3, x+6, y+3, x+10, y-5, color); spr->drawLine(x-10, y-5, x+8, y+5, COLOR_DANGER); break;
        default: break;
    }
}