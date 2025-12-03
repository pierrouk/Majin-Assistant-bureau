#include "CountdownWidget.h"

void CountdownWidget::begin(CoreManager* core, NetworkManager* net, SettingsManager* settings) {
    _settings = settings;
    _net = net;
}

void CountdownWidget::update() {
    // Rien de spécial
}

void CountdownWidget::draw(LGFX_Sprite* spr, int x, int y) {
    // On n'affiche le widget que si connecté (pour avoir l'heure juste) 
    // et si un nom d'événement est défini.
    if (!_net->isConnected()) return;
    
    String evtName = _settings->getEventName();
    if (evtName.length() == 0) return;

    int type = _settings->getEventType();
    
    // Calcul du temps restant
    time_t now; 
    time(&now);
    unsigned long target = _settings->getEventTimestamp();
    
    long diff = target - (unsigned long)now;
    if (diff < 0) diff = 0;
    long d = diff / 86400;

    // Dessin de l'icône à gauche
    _drawEventIcon(spr, type, x + 8, y + 10, COLOR_WHITE);

    // Dessin du Texte
    int textX = x + 25;
    
    spr->setTextDatum(top_left);
    spr->setFont(FONT_SMALL); 
    spr->setTextColor(0xCE79); // Gris/Vert
    
    // Tronquer le nom si trop long pour le widget
    if(evtName.length() > 10) evtName = evtName.substring(0, 10) + ".";
    spr->drawString(evtName, textX, y + 2);
    
    // Jours restants en gros
    spr->setFont(FONT_UI); 
    spr->setTextColor(COLOR_WHITE);
    spr->drawString("J-" + String(d), textX, y + 12);
}

// Helper graphique local
void CountdownWidget::_drawEventIcon(LGFX_Sprite* spr, int type, int x, int y, uint16_t color) {
    switch (type) {
        case 1: // Anniversaire
            spr->fillRect(x-6, y, 12, 6, 0xFCA0); 
            spr->fillRect(x-6, y-3, 12, 3, 0xF80A); 
            spr->drawLine(x, y-3, x, y-7, 0xFFE0); spr->drawPixel(x, y-8, 0xF800);
            break;
        case 2: // Noël
            spr->fillTriangle(x, y-8, x-6, y+4, x+6, y+4, 0x07E0);
            spr->fillRect(x-1, y+4, 2, 2, 0x9280); 
            break;
        case 3: // Vacances (Soleil)
            spr->fillCircle(x, y, 6, 0xFFE0); spr->drawCircle(x, y, 6, 0xFCA0); 
            spr->drawLine(x, y-8, x, y+8, 0xFFE0); spr->drawLine(x-8, y, x+8, y, 0xFFE0);
            break;
        case 4: // Mariage (Bague)
            spr->drawCircle(x, y+2, 5, 0xFFE0); spr->fillCircle(x, y-4, 3, 0x07FF); spr->drawPixel(x-1, y-5, COLOR_WHITE);
            break;
        case 5: // Montagne
            spr->fillTriangle(x-8, y+6, x-2, y-4, x+4, y+6, 0x7BEF); 
            spr->fillTriangle(x, y+6, x+4, y, x+8, y+6, 0x3333); 
            spr->fillTriangle(x-2, y-4, x-3, y-2, x-1, y-2, COLOR_WHITE); 
            break;
        default: // Horloge
            spr->drawCircle(x, y, 6, color); 
            spr->drawLine(x, y, x, y-4, color); spr->drawLine(x, y, x+3, y, color);
            break;
    }
}