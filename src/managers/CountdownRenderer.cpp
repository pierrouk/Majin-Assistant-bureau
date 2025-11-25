#include "CountdownRenderer.h"

CountdownRenderer::CountdownRenderer() {}

void CountdownRenderer::begin(CoreManager* core, SettingsManager* settings) {
    _core = core;
    _settings = settings;
}

void CountdownRenderer::draw(LGFX_Sprite* spr) {
    // 1. Fond
    spr->fillScreen(COLOR_BG);

    // Récupération données
    String name = _settings->getEventName();
    unsigned long target = _settings->getEventTimestamp();
    int type = _settings->getEventType();

    time_t now; 
    time(&now);
    long remaining = target - now;

    // Centre horizontal
    int cx = 160;

    // --- A. ICÔNE EN HAUT (Remontée un peu : Y=40) ---
    int iconY = 40;
    switch (type) {
        case 1: _drawIconBirthday(spr, cx, iconY); break;
        case 2: _drawIconXmas(spr, cx, iconY); break;
        case 3: _drawIconVacation(spr, cx, iconY); break;
        case 4: _drawIconWedding(spr, cx, iconY); break;
        case 5: _drawIconMountain(spr, cx, iconY); break;
        default: _drawIconGeneric(spr, cx, iconY); break;
    }

    // --- B. COMPTEUR PRINCIPAL ---
    if (remaining <= 0) {
        spr->setTextColor(COLOR_SUCCESS);
        spr->setFont(FONT_TITLE);
        spr->setTextSize(2);
        spr->setTextDatum(middle_center);
        spr->drawString("JOUR J !", cx, 120);
        
        spr->setFont(FONT_UI);
        spr->setTextSize(1);
        spr->setTextColor(COLOR_WHITE);
        spr->drawString("Evenement en cours", cx, 160);
    } else {
        int days = remaining / 86400;
        int hours = (remaining % 86400) / 3600;
        int mins = (remaining % 3600) / 60;
        int secs = remaining % 60;
        
        // 1. Nombre de Jours (GROS) - Y=100
        spr->setTextColor(COLOR_WHITE);
        spr->setFont(FONT_TITLE);
        spr->setTextSize(3); 
        spr->setTextDatum(middle_center);
        spr->drawString(String(days), cx, 100);

        // 2. Label "JOURS" - Y=135
        spr->setFont(FONT_UI);
        spr->setTextSize(1);
        spr->setTextColor(0xCE79); // Gris clair
        spr->drawString("JOURS", cx, 135);

        // 3. Décompte Précis (H:M:S) - Y=165 ⬅️ AJOUTÉ
        char timeBuf[32];
        sprintf(timeBuf, "%02d h %02d m %02d s", hours, mins, secs);
        
        spr->setTextColor(COLOR_WHITE);
        spr->setFont(FONT_UI); // Police moyenne lisible
        spr->drawString(String(timeBuf), cx, 165);
        
        // Barre de progression secondes (Optionnel, pour le dynamisme)
        int barW = map(secs, 0, 60, 0, 200);
        spr->fillRect(cx - 100, 178, barW, 4, COLOR_ACCENT);
    }

    // --- C. TEXTE EN BAS (Y=210) ---
    spr->setFont(FONT_UI);
    spr->setTextSize(1);
    spr->setTextDatum(middle_center);
    
    // Ligne 1 : "avant"
    spr->setTextColor(0x5555); // Gris très sombre
    spr->drawString("avant", cx, 195);

    // Ligne 2 : NOM DE L'EVENT
    spr->setTextColor(COLOR_PRIMARY); // Cyan
    spr->drawString(name, cx, 215);
}

// --- DESSIN DES ICÔNES (Pixel Art Vectoriel) ---

void CountdownRenderer::_drawIconGeneric(LGFX_Sprite* spr, int x, int y) {
    spr->fillCircle(x, y, 6, COLOR_PRIMARY);
    spr->fillTriangle(x, y-25, x-6, y, x+6, y, COLOR_PRIMARY); 
    spr->fillTriangle(x, y+25, x-6, y, x+6, y, COLOR_PRIMARY); 
    spr->fillTriangle(x-25, y, x, y-6, x, y+6, COLOR_PRIMARY); 
    spr->fillTriangle(x+25, y, x, y-6, x, y+6, COLOR_PRIMARY); 
    spr->fillCircle(x, y, 3, COLOR_WHITE);
}

void CountdownRenderer::_drawIconBirthday(LGFX_Sprite* spr, int x, int y) {
    spr->fillRect(x-20, y, 40, 20, 0xE463); 
    spr->fillRect(x-20, y-10, 40, 10, 0xF80A); 
    spr->fillRect(x-5, y-20, 2, 10, COLOR_WHITE);
    spr->fillCircle(x-4, y-22, 2, 0xFCE0); 
}

void CountdownRenderer::_drawIconXmas(LGFX_Sprite* spr, int x, int y) {
    spr->fillTriangle(x, y-30, x-25, y+10, x+25, y+10, 0x07E0); 
    spr->fillRect(x-5, y+10, 10, 10, 0xA145); 
    spr->fillCircle(x-10, y-5, 3, COLOR_DANGER);
    spr->fillCircle(x+12, y+5, 3, COLOR_PRIMARY);
}

void CountdownRenderer::_drawIconVacation(LGFX_Sprite* spr, int x, int y) {
    spr->fillCircle(x, y, 18, 0xFFE0); 
    for(int i=0; i<8; i++) {
        float angle = i * (2*PI/8);
        int x1 = x + cos(angle)*22; int y1 = y + sin(angle)*22;
        int x2 = x + cos(angle)*30; int y2 = y + sin(angle)*30;
        spr->drawLine(x1, y1, x2, y2, 0xFFE0);
    }
}

void CountdownRenderer::_drawIconWedding(LGFX_Sprite* spr, int x, int y) {
    spr->drawCircle(x, y+5, 15, 0xFFE0); 
    spr->drawCircle(x, y+5, 14, 0xFFE0); 
    spr->fillTriangle(x-8, y-10, x+8, y-10, x, y+2, COLOR_PRIMARY);
    spr->fillTriangle(x-8, y-10, x+8, y-10, x, y-15, 0xFFFF); 
}

void CountdownRenderer::_drawIconMountain(LGFX_Sprite* spr, int x, int y) {
    spr->fillTriangle(x-30, y+20, x-10, y-20, x+10, y+20, 0x8410); 
    spr->fillTriangle(x-10, y+20, x+15, y-30, x+40, y+20, 0xFFFF); 
    spr->fillTriangle(x-10, y-20, x-15, y-10, x-5, y-10, COLOR_WHITE);
}