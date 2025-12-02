#include "UIManager.h"
#include "../../include/Theme.h" 
#include <time.h> 

#ifndef DEG_TO_RAD
#define DEG_TO_RAD 0.017453292519943295769236907684886
#endif

UIManager::UIManager() : _mainSprite(nullptr) {
}

void UIManager::begin(DisplayDriver* display, CoreManager* core, NetworkManager* net, SettingsManager* settings, UsbManager* usb) {
    _display = display; _core = core; _net = net; _settings = settings; _usb = usb;
    
    _mainSprite.setPsram(true); _mainSprite.setColorDepth(16); _mainSprite.createSprite(320, 240);
    _mainSprite.setFont(FONT_UI);

    _face.begin(core, settings);
    _countdown.begin(core, settings); 

    _initApps();
    _currentScene = SCENE_FACE; 
}

void UIManager::_initApps() {
    _apps.push_back({"Souris", COLOR_PRIMARY, 80,  100, 30, SCENE_APP_TRACKPAD});
    _apps.push_back({"Deck",   COLOR_ACCENT,  240, 100, 30, SCENE_APP_STREAMDECK});
    _apps.push_back({"Config", 0xFCA0,        80,  200, 30, SCENE_SETTINGS}); 
    _apps.push_back({"System", COLOR_SUCCESS, 240, 200, 30, SCENE_SETTINGS});
}

// --- HELPER DECK ---
static void _drawDeckIcon(LGFX_Sprite* spr, int id, int x, int y, uint16_t color) {
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
        default: break;
    }
}

// --- HELPER EVENT (Corrigé) ---
static void _drawEventIcon(LGFX_Sprite* spr, int type, int x, int y, uint16_t color) {
    switch (type) {
        case 1: // Anniversaire
            spr->fillRect(x-6, y, 12, 6, 0xFCA0); 
            spr->fillRect(x-6, y-3, 12, 3, 0xF80A); 
            spr->drawLine(x, y-3, x, y-7, 0xFFE0); spr->drawPixel(x, y-8, 0xF800);
            break;
        case 2: // Noël
            spr->fillTriangle(x, y-8, x-6, y+4, x+6, y+4, 0x07E0);
            spr->fillRect(x-1, y+4, 2, 2, 0x9280); spr->drawPixel(x, y-8, 0xFFE0);
            break;
        case 3: // Vacances
            spr->fillCircle(x, y, 6, 0xFFE0); spr->drawCircle(x, y, 6, 0xFCA0); 
            spr->drawLine(x, y-8, x, y+8, 0xFFE0); spr->drawLine(x-8, y, x+8, y, 0xFFE0);
            break;
        case 4: // Mariage
            spr->drawCircle(x, y+2, 5, 0xFFE0); spr->fillCircle(x, y-4, 3, 0x07FF); spr->drawPixel(x-1, y-5, COLOR_WHITE);
            break;
        case 5: // Montagne
            spr->fillTriangle(x-8, y+6, x-2, y-4, x+4, y+6, 0x7BEF); 
            spr->fillTriangle(x, y+6, x+4, y, x+8, y+6, 0x3333); 
            spr->fillTriangle(x-2, y-4, x-3, y-2, x-1, y-2, COLOR_WHITE); 
            break;
        default: // Horloge
            spr->drawCircle(x, y, 6, color); spr->drawLine(x, y, x, y-4, color); spr->drawLine(x, y, x+3, y, color);
            break;
    }
}

// --- DRAW LOOP ---
void UIManager::draw() {
    if (_currentScene != SCENE_FACE) { _mainSprite.fillScreen(COLOR_BG); }

    switch (_currentScene) {
        case SCENE_FACE:      _face.draw(&_mainSprite); break;
        case SCENE_COUNTDOWN: _countdown.draw(&_mainSprite); break;
        case SCENE_MENU:      _drawSceneMenu(); break;
        case SCENE_APP_TRACKPAD: _drawAppTrackpad(); break;     
        case SCENE_APP_STREAMDECK: _drawAppStreamDeck(); break;
        case SCENE_SETUP_WIFI: _drawSetupWifi(); break;
        case SCENE_TAMA_MENU: _drawTamaMenu(); break; 
        case SCENE_SETTINGS: _mainSprite.drawString("REGLAGES (WIP)", 160, 120); break;
        default: break;
    }
    
    _drawParticles();

    if (_currentScene == SCENE_FACE || _currentScene == SCENE_COUNTDOWN || _currentScene == SCENE_APP_TRACKPAD) {
        _drawUnifiedOverlay();
    }

    if (_notifMsg != "") _drawNotification();
    if (_usb->isJigglerActive() && _currentScene == SCENE_FACE) _drawMouseStatus();

    _display->secureDraw([&](LGFX_Majin* tft) {
        _mainSprite.pushSprite(tft, 0, 0);
    });
}

// --- OVERLAY ---
void UIManager::_drawUnifiedOverlay() {
    if (!_settings->isSetupDone()) return;
    _drawStatusBar();     
    _drawBottomWidgets(); 
}

void UIManager::_drawStatusBar() {
    int y = 5;
    if (_settings->getShowSensors()) {
        _mainSprite.setFont(FONT_SMALL); _mainSprite.setTextDatum(top_left); _mainSprite.setTextColor(COLOR_PRIMARY); 
        String tempStr = String(_core->getTemp(), 1) + "C"; _mainSprite.drawString(tempStr, 5, y); 
    }
    if (_settings->getShowTime()) {
        _mainSprite.setFont(FONT_UI); _mainSprite.setTextColor(COLOR_WHITE); _mainSprite.setTextDatum(top_center); 
        String timeStr = _net->isConnected() ? _core->getTimeString().substring(0, 5) : "--:--";
        _mainSprite.drawString(timeStr, 160, y); 
    }
    _drawWiFiIcon(305, y + 4, _net->isConnected());
}

void UIManager::_drawWiFiIcon(int x, int y, bool connected) {
    uint16_t color = connected ? COLOR_WHITE : 0x5555; 
    _mainSprite.drawArc(x, y+6, 8, 6, 225, 315, color);
    _mainSprite.drawArc(x, y+6, 5, 4, 225, 315, color);
    _mainSprite.fillCircle(x, y+6, 1, color);
    if (!connected) { _mainSprite.drawLine(x-6, y-2, x+6, y+8, COLOR_DANGER); }
}

void UIManager::_drawBottomWidgets() {
    if (_settings->getWeatherEnabled()) { _drawWeatherWidget(10, 200); }
    if (_net->isConnected()) {
        String evtName = _settings->getEventName();
        if (evtName.length() > 0) { _drawCountdownWidget(220, 205); }
    }
}

void UIManager::_drawCountdownWidget(int x, int y) {
    String evtName = _settings->getEventName();
    int type = _settings->getEventType();
    time_t now; time(&now);
    unsigned long target = _settings->getEventTimestamp();
    long diff = target - (unsigned long)now;
    if (diff < 0) diff = 0;
    long d = diff / 86400;

    _drawEventIcon(&_mainSprite, type, x + 8, y + 10, COLOR_WHITE);

    int textX = x + 25;
    _mainSprite.setTextDatum(top_left);
    _mainSprite.setFont(FONT_SMALL); _mainSprite.setTextColor(0xCE79);
    if(evtName.length() > 10) evtName = evtName.substring(0, 10) + ".";
    _mainSprite.drawString(evtName, textX, y + 2);
    _mainSprite.setFont(FONT_UI); _mainSprite.setTextColor(COLOR_WHITE);
    _mainSprite.drawString("J-" + String(d), textX, y + 12);
}

void UIManager::_drawWeatherWidget(int x, int y) {
    int code = _core->getExternalWeatherCode();
    float temp = _core->getExternalTemp();
    String city = _settings->getCityName();
    if (code == -1) return;

    _mainSprite.setTextDatum(top_left);
    int ix = x + 20; int iy = y + 15;
    uint16_t sunCol = 0xFFE0; uint16_t cloudCol = 0xCE79; uint16_t rainCol = 0x001F;

    if (code == 0) { 
        _mainSprite.fillCircle(ix, iy, 7, sunCol); 
        for(int i=0; i<8; i++) { float a = i * (PI/4); _mainSprite.drawLine(ix + cos(a)*9, iy + sin(a)*9, ix + cos(a)*12, iy + sin(a)*12, sunCol); }
    } else if (code <= 3) { 
        _mainSprite.fillCircle(ix+4, iy-4, 5, sunCol); _mainSprite.fillCircle(ix-3, iy+2, 6, COLOR_WHITE); _mainSprite.fillCircle(ix+5, iy+3, 5, COLOR_WHITE); 
    } else if (code <= 48) { 
        _mainSprite.fillCircle(ix-4, iy+2, 6, cloudCol); _mainSprite.fillCircle(ix+4, iy, 7, cloudCol); _mainSprite.fillCircle(ix+2, iy+5, 6, cloudCol);
    } else if ((code >= 51 && code <= 67) || (code >= 80 && code <= 82)) { 
        _mainSprite.fillCircle(ix-4, iy, 6, cloudCol); _mainSprite.fillCircle(ix+4, iy-2, 7, cloudCol); 
        _mainSprite.drawLine(ix-2, iy+8, ix-4, iy+13, rainCol); _mainSprite.drawLine(ix+2, iy+8, ix, iy+13, rainCol); _mainSprite.drawLine(ix+6, iy+8, ix+4, iy+13, rainCol);
    } else if (code >= 71 && code <= 86) { 
        _mainSprite.fillCircle(ix, iy, 8, cloudCol); _mainSprite.fillCircle(ix-3, iy+10, 2, COLOR_WHITE); _mainSprite.fillCircle(ix+3, iy+8, 2, COLOR_WHITE);
    } else if (code >= 95) { 
        _mainSprite.fillCircle(ix, iy, 8, 0x3333); _mainSprite.fillTriangle(ix-2, iy+8, ix+2, iy+8, ix-4, iy+16, sunCol);
    }

    int textX = x + 45; 
    _mainSprite.setFont(FONT_UI); _mainSprite.setTextColor(COLOR_WHITE); _mainSprite.setTextDatum(bottom_left); _mainSprite.drawString(String(temp, 1) + "C", textX, y + 18);
    _mainSprite.setFont(FONT_SMALL); _mainSprite.setTextColor(0xCE79); _mainSprite.setTextDatum(top_left); 
    if(city.length() > 9) city = city.substring(0,9); _mainSprite.drawString(city, textX, y + 20);
}

// --- DESSINS SCÈNES ---

void UIManager::_drawSetupWifi() { 
    _mainSprite.fillScreen(COLOR_UI_BG); 
    if (_settings->isSetupDone()) {
        _mainSprite.setTextColor(COLOR_WHITE); _mainSprite.setTextDatum(top_center); _mainSprite.setFont(FONT_TITLE); _mainSprite.drawString("INFOS RESEAU", 160, 20); 
        _mainSprite.drawRoundRect(20, 60, 280, 100, 8, COLOR_PRIMARY);
        _mainSprite.setFont(FONT_UI); _mainSprite.setTextDatum(middle_center);
        String ssid = WiFi.SSID(); String ip = _net->getIP();
        _mainSprite.setTextColor(COLOR_ACCENT); _mainSprite.drawString("WiFi: " + ssid, 160, 90);
        _mainSprite.setTextColor(COLOR_SUCCESS); _mainSprite.drawString("IP: " + ip, 160, 120);
        _mainSprite.setFont(FONT_SMALL); _mainSprite.setTextColor(0xCE79); _mainSprite.setTextDatum(bottom_center); _mainSprite.drawString("Swipe ou Tap pour fermer", 160, 220);
    } else {
        _mainSprite.fillRoundRect(160 - 20, 60, 15, 40, 5, COLOR_WHITE); _mainSprite.fillRoundRect(160 + 5, 60, 15, 40, 5, COLOR_WHITE); 
        _mainSprite.setTextColor(COLOR_WHITE); _mainSprite.setTextDatum(top_center); _mainSprite.setFont(FONT_TITLE); _mainSprite.drawString("CONFIGURATION", 160, 110); 
        _mainSprite.setFont(FONT_SMALL); _mainSprite.setTextColor(COLOR_PRIMARY); _mainSprite.drawString("1. Connectez-vous au WiFi:", 160, 150); 
        _mainSprite.setFont(FONT_TITLE); _mainSprite.setTextColor(COLOR_ACCENT); _mainSprite.drawString("Majin_Setup", 160, 175); 
        _mainSprite.setFont(FONT_SMALL); _mainSprite.setTextColor(COLOR_WHITE); _mainSprite.drawString("2. Configurez le réseau", 160, 210); 
    }
}

// 🕸️ MENU HEXAGONAL (Mode Densité - Voisins Visibles) 🕸️
void UIManager::_drawSceneMenu() { 
    _mainSprite.fillScreen(COLOR_BG); 
    
    _mainSprite.setTextDatum(top_center); 
    _mainSprite.setTextColor(COLOR_WHITE); 
    _mainSprite.setFont(FONT_TITLE); 
    _mainSprite.drawString("APPS", 160, 20); 

    int centerX = 160; 
    int centerY = 120; 
    
    // 👇 RÉGLAGES "2-3 ICONES"
    int spacing = 100; // Plus serré (Avant 90, mais avec moins de zoom)
    int baseRadius = 38; // Taille de base un peu plus petite pour en caser plus

    for (int i = 0; i < _apps.size(); i++) {
        float offset = (i - _menuScrollCurrent);
        int x = centerX + (offset * spacing);
        
        // Petit effet de vague (Sinus) sur la hauteur pour le style
        int y = centerY + (sin(offset) * 10);

        // Zone de visibilité élargie
        if (x > -50 && x < 370) {
            float dist = abs(offset);
            
            // ZOOM PLUS DOUX : Les voisins restent gros (90% de la taille)
            float scale = (dist < 0.5) ? 1.0 : 0.9; 
            int radius = baseRadius * scale;
            
            uint32_t color = _apps[i].color;
            
            // LUMINOSITÉ : On ne fonce presque pas les voisins pour qu'ils soient lisibles
            if (dist > 0.8) { 
                // Légèrement plus sombre seulement si vraiment loin
                color = (color & 0xF7DE) >> 1; 
            }

            // Dessin
            _drawHexagon(&_mainSprite, x, y, radius, color, true);
            
            // Contour : Blanc si sélectionné, sinon couleur de l'app
            uint32_t outline = (dist < 0.4) ? COLOR_WHITE : color;
            // Contour un peu plus épais pour le style
            _drawHexagon(&_mainSprite, x, y, radius + 2, outline, false);
            if(dist < 0.4) _drawHexagon(&_mainSprite, x, y, radius + 3, outline, false); // Double contour sélection

            // TEXTE : Affiché sur le centre ET les voisins proches
            if (dist < 1.2) {
                _mainSprite.setFont(FONT_SMALL); 
                _mainSprite.setTextColor(COLOR_WHITE); 
                _mainSprite.setTextDatum(middle_center); 
                _mainSprite.drawString(_apps[i].name, x, y);
            }
        }
    }
    
    // Points de navigation
    int dotY = 220; 
    int totalW = _apps.size() * 15; 
    int startDotX = 160 - (totalW / 2);
    for (int i = 0; i < _apps.size(); i++) {
        uint16_t c = (i == _menuScrollTarget) ? COLOR_PRIMARY : 0x3333;
        _mainSprite.fillCircle(startDotX + (i * 15), dotY, 3, c);
    }
}

// 🕸️ HELPER HEXAGON
void UIManager::_drawHexagon(LGFX_Sprite* spr, int x, int y, int radius, uint32_t color, bool filled) {
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

void UIManager::_drawAppTrackpad() { 
    _mainSprite.fillScreen(0x3333); _mainSprite.drawRect(10, 10, 300, 220, COLOR_WHITE); 
    _mainSprite.setTextColor(COLOR_WHITE); _mainSprite.setTextDatum(middle_center); _mainSprite.setFont(FONT_TITLE); _mainSprite.drawString("TRACKPAD", 160, 100); 
    _mainSprite.setFont(FONT_UI); _mainSprite.setTextColor(COLOR_PRIMARY); _mainSprite.drawString("Zone Active", 160, 140); 
    _mainSprite.setFont(FONT_SMALL); _mainSprite.setTextColor(0xCE79); _mainSprite.drawString("Appui Long pour quitter", 160, 200); 
}

void UIManager::_drawAppStreamDeck() {
    _mainSprite.fillScreen(COLOR_BG); 
    _mainSprite.setTextColor(COLOR_WHITE); _mainSprite.setFont(FONT_TITLE); _mainSprite.drawString("STREAM DECK", 160, 20); _mainSprite.setFont(FONT_UI); 
    int startX = 25; int startY = 50; int btnW = 80; int btnH = 60; int gap = 15; int startIndex = _currentDeckPage * 6; 
    for(int i=0; i<6; i++) { 
        int col = i % 3; int row = i / 3; int x = startX + col * (btnW + gap); int y = startY + row * (btnH + gap); DeckButton btn = _settings->getDeckButton(startIndex + i); 
        if (btn.active) { 
            _mainSprite.fillRoundRect(x, y, btnW, btnH, 8, btn.color); _mainSprite.drawRoundRect(x, y, btnW, btnH, 8, COLOR_WHITE); 
            if (btn.iconID > 0) { _drawDeckIcon(&_mainSprite, btn.iconID, x + btnW/2, y + btnH/2 - 5, COLOR_BLACK); _mainSprite.setTextDatum(bottom_center); _mainSprite.setTextColor(COLOR_BLACK); _mainSprite.setFont(FONT_SMALL); _mainSprite.drawString(btn.label, x + btnW/2, y + btnH - 5); } 
            else { _mainSprite.setTextDatum(middle_center); _mainSprite.setTextColor(COLOR_BLACK); _mainSprite.setFont(FONT_UI); _mainSprite.drawString(btn.label, x + btnW/2, y + btnH/2); } 
        } else { _mainSprite.drawRoundRect(x, y, btnW, btnH, 8, 0x3333); _mainSprite.setTextColor(0x5555); _mainSprite.setTextDatum(middle_center); _mainSprite.drawString("+", x + btnW/2, y + btnH/2); } 
    } 
    int dotY = 200; for(int p=0; p<4; p++) { uint16_t color = (p == _currentDeckPage) ? COLOR_PRIMARY : 0x3333; _mainSprite.fillCircle(160 - 30 + (p*20), dotY, 4, color); } 
    _mainSprite.setFont(FONT_SMALL); _mainSprite.setTextColor(0xCE79); _mainSprite.setTextDatum(bottom_center); _mainSprite.drawString("Swipe < >", 160, 230); 
}

void UIManager::_drawTamaMenu() { 
    _mainSprite.fillScreen(0x0804); _mainSprite.setTextDatum(top_center); _mainSprite.setTextColor(COLOR_WHITE); _mainSprite.setFont(FONT_TITLE); _mainSprite.drawString("SOINS", 160, 15); 
    int btnY = 140; int barY = 85; int barW = 50; int barH = 6; _mainSprite.setFont(FONT_SMALL); 
    int hunger = _core->getHunger(); int satFill = map(100 - hunger, 0, 100, 0, barW); _mainSprite.setTextColor(0xE463); _mainSprite.drawString("Faim", 60, barY - 15); _mainSprite.drawRect(60 - barW/2 - 1, barY - 1, barW + 2, barH + 2, COLOR_WHITE); _mainSprite.fillRect(60 - barW/2, barY, barW, barH, 0x2124); _mainSprite.fillRect(60 - barW/2, barY, satFill, barH, 0xE463); _mainSprite.fillCircle(60, btnY, 35, 0xE463); _mainSprite.setFont(FONT_UI); _mainSprite.setTextColor(COLOR_WHITE); _mainSprite.setTextDatum(middle_center); _mainSprite.drawString("Snack", 60, btnY + 50); int bx = 50; int by = btnY - 5; _mainSprite.fillRoundRect(bx, by, 20, 4, 2, 0xA145); _mainSprite.fillRect(bx, by+5, 20, 2, 0x2580); _mainSprite.fillRect(bx, by+8, 20, 3, 0x7000); _mainSprite.fillRoundRect(bx, by+12, 20, 4, 2, 0xA145); 
    int energy = _core->getEnergy(); int nrgFill = map(energy, 0, 100, 0, barW); uint16_t nrgCol = (energy < 30) ? COLOR_DANGER : COLOR_SUCCESS; _mainSprite.setFont(FONT_SMALL); _mainSprite.setTextColor(nrgCol); _mainSprite.setTextDatum(top_center); _mainSprite.drawString("Energie", 160, barY - 15); _mainSprite.drawRect(160 - barW/2 - 1, barY - 1, barW + 2, barH + 2, COLOR_WHITE); _mainSprite.fillRect(160 - barW/2, barY, barW, barH, 0x2124); _mainSprite.fillRect(160 - barW/2, barY, nrgFill, barH, nrgCol); uint16_t sleepCol = (_core->getMood() == MOOD_SLEEP) ? COLOR_SUCCESS : COLOR_PRIMARY; _mainSprite.fillCircle(160, btnY, 35, sleepCol); _mainSprite.setFont(FONT_UI); _mainSprite.setTextColor(COLOR_WHITE); _mainSprite.setTextDatum(middle_center); _mainSprite.drawString("Dodo", 160, btnY + 50); _mainSprite.drawString("Zzz", 160, btnY); 
    int fun = _core->getFun(); int funFill = map(fun, 0, 100, 0, barW); _mainSprite.setFont(FONT_SMALL); _mainSprite.setTextColor(COLOR_ACCENT); _mainSprite.setTextDatum(top_center); _mainSprite.drawString("Fun", 260, barY - 15); _mainSprite.drawRect(260 - barW/2 - 1, barY - 1, barW + 2, barH + 2, COLOR_WHITE); _mainSprite.fillRect(260 - barW/2, barY, barW, barH, 0x2124); _mainSprite.fillRect(260 - barW/2, barY, funFill, barH, COLOR_ACCENT); _mainSprite.fillCircle(260, btnY, 35, COLOR_ACCENT); _mainSprite.setFont(FONT_UI); _mainSprite.setTextColor(COLOR_WHITE); _mainSprite.setTextDatum(middle_center); _mainSprite.drawString("Jeu", 260, btnY + 50); _mainSprite.fillTriangle(255, btnY-10, 255, btnY+10, 270, btnY, COLOR_WHITE); 
    _mainSprite.setFont(FONT_SMALL); _mainSprite.setTextDatum(bottom_center); _mainSprite.setTextColor(0xCE79); _mainSprite.drawString("Swipe UP pour retour", 160, 235); 
}

// --- LOGIQUE & INTERACTIONS ---
void UIManager::setScene(UIScene scene) { 
    if (scene == SCENE_COUNTDOWN) { 
        int type = _settings->getEventType(); 
        switch(type) { 
            case 1: setEffect(EFFECT_CONFETTI); break; 
            case 2: setEffect(EFFECT_SNOW); break; 
            case 3: setEffect(EFFECT_SUN); break; 
            case 4: setEffect(EFFECT_SAKURA); break; 
            case 5: setEffect(EFFECT_SNOW); break; // Montagne -> Neige
            default: setEffect(EFFECT_NONE); break; 
        } 
    } else { setEffect(EFFECT_NONE); } 
    _currentScene = scene; 
    if (scene == SCENE_COUNTDOWN) { _sceneStartTime = millis(); } 
}

UIScene UIManager::getScene() { return _currentScene; }
void UIManager::showNotification(String message, uint32_t color, int durationMs) { _notifMsg = message; _notifColor = color; _notifDuration = durationMs; _notifStartTime = millis(); }

// 💬 NOTIFICATION TYPE BULLE
void UIManager::_drawNotification() { 
    int x = 220; int y = 60; int w = 140; int h = 50; int r = 10;
    int baseX1 = x - 40; int baseX2 = x - 10; int baseY  = y + 15; int tipX = 160; int tipY = 100;
    _mainSprite.fillTriangle(baseX1, baseY, baseX2, baseY, tipX, tipY, COLOR_WHITE);
    _mainSprite.fillRoundRect(x - w/2, y - h/2, w, h, r, COLOR_WHITE);
    _mainSprite.drawRoundRect(x - w/2, y - h/2, w, h, r, _notifColor);
    _mainSprite.setFont(FONT_UI); _mainSprite.setTextDatum(middle_center); 
    uint16_t textColor = (_notifColor == COLOR_WHITE) ? COLOR_BLACK : _notifColor;
    if (_notifColor == COLOR_PRIMARY) textColor = COLOR_BLACK;
    _mainSprite.setTextColor(textColor); 
    if (_mainSprite.textWidth(_notifMsg) > (w - 10)) { _mainSprite.setFont(FONT_SMALL); }
    _mainSprite.drawString(_notifMsg, x, y); 
}

void UIManager::_drawMouseStatus() { int x = 20; int y = 220; _mainSprite.fillRoundRect(x, y, 16, 22, 6, COLOR_PRIMARY); _mainSprite.drawLine(x+8, y, x+8, y+10, COLOR_BG); _mainSprite.fillRect(x+6, y+4, 4, 6, COLOR_BG); _mainSprite.drawArc(x+20, y, 6, 5, 30, 150, COLOR_PRIMARY); }

void UIManager::update() {
    _updateParticles();
    
    // Animation Menu Hexagonal
    if (_currentScene == SCENE_MENU) {
        _menuScrollCurrent += (_menuScrollTarget - _menuScrollCurrent) * 0.2;
    }

    if (_currentScene == SCENE_FACE || _currentScene == SCENE_SETUP_WIFI) { _face.update(); }
    if (_currentScene == SCENE_COUNTDOWN) { if (millis() - _sceneStartTime > SCENE_TIMEOUT) setScene(SCENE_FACE); }
    if (_notifDuration > 0 && (millis() - _notifStartTime > _notifDuration)) { _notifMsg = ""; _notifDuration = 0; }
}

// 🕸️ INTERACTION MENU HEXAGONAL
bool UIManager::handleMenuClick(int touchX, int touchY) { 
    if (_currentScene != SCENE_MENU) return false; 
    
    // Zone Centrale (Validation)
    if (touchX > 110 && touchX < 210 && touchY > 80 && touchY < 160) {
        AppIcon currentApp = _apps[_menuScrollTarget];
        if (currentApp.name == "Souris") { 
            bool newState = !_usb->isJigglerActive(); _usb->setJiggler(newState); 
            if (newState) showNotification("SOURIS AUTO", COLOR_PRIMARY, 2000); 
            else showNotification("ARRET SOURIS", 0xCE79, 2000); setScene(SCENE_FACE); 
        } else { setScene(currentApp.targetScene); }
        return true;
    }
    // Zone Gauche (Scroll)
    if (touchX < 100) { if (_menuScrollTarget > 0) _menuScrollTarget--; return true; }
    // Zone Droite (Scroll)
    if (touchX > 220) { if (_menuScrollTarget < _apps.size() - 1) _menuScrollTarget++; return true; }
    return false; 
}

TamaAction UIManager::handleTamagotchiClick(int touchX, int touchY) { 
    if (_currentScene != SCENE_TAMA_MENU) return ACTION_NONE; 
    int btnY = 140; int tolerance = 40; 
    if (abs(touchX - 60) < tolerance && abs(touchY - btnY) < tolerance) { _core->feed(20); return ACTION_FEED; } 
    if (abs(touchX - 160) < tolerance && abs(touchY - btnY) < tolerance) { if (_core->getMood() == MOOD_SLEEP) { _core->wakeUp(); return ACTION_WAKE; } else { _core->sleep(true); return ACTION_SLEEP; } } 
    if (abs(touchX - 260) < tolerance && abs(touchY - btnY) < tolerance) { _core->play(15); return ACTION_PLAY; } 
    return ACTION_NONE; 
}

void UIManager::nextDeckPage() { _currentDeckPage++; if (_currentDeckPage >= 4) _currentDeckPage = 0; }
void UIManager::prevDeckPage() { _currentDeckPage--; if (_currentDeckPage < 0) _currentDeckPage = 3; }

void UIManager::setEffect(UIEffect effect) { if (_currentEffect != effect) { _currentEffect = effect; _initParticles(effect); } }
void UIManager::setWeatherEffect(UIEffect effect) { _weatherEffect = effect; if (_currentScene != SCENE_COUNTDOWN && _currentScene != SCENE_FACE) { setEffect(effect); } } 

void UIManager::_initParticles(UIEffect type) { _particles.clear(); if (type == EFFECT_NONE) return; int count = MAX_PARTICLES; if (type == EFFECT_CLOUDS) count = 15; for (int i = 0; i < count; i++) { Particle p; p.x = random(0, 320); p.y = random(-240, 0); p.active = true; switch (type) { case EFFECT_SNOW: p.speedX = random(-10, 10)/10.0; p.speedY = random(10, 30)/10.0; p.size = random(1, 3); p.color = COLOR_WHITE; break; case EFFECT_RAIN: p.speedX = -0.5; p.speedY = random(40, 70)/10.0; p.size = random(5, 10); p.color = 0x001F; break; case EFFECT_CONFETTI: p.speedX = random(-20, 20)/10.0; p.speedY = random(20, 50)/10.0; p.size = random(2, 5); p.color = (random(2))?COLOR_PRIMARY:(random(2)?COLOR_ACCENT:COLOR_SUCCESS); if(random(10)>8) p.color = 0xFFE0; break; case EFFECT_SAKURA: p.speedX = random(-15, 15)/10.0; p.speedY = random(10, 25)/10.0; p.size = random(2, 4); p.color = 0xF63D; break; case EFFECT_CLOUDS: p.y = random(0, 100); p.speedX = random(2, 8) / 10.0; p.speedY = 0; p.size = random(20, 40); p.color = 0x3186; break; case EFFECT_SUN: p.x = random(0, 320); p.y = random(0, 240); p.speedX = 0; p.speedY = 0; p.size = random(1, 3); p.color = 0xFFE0; break; default: break; } _particles.push_back(p); } }
void UIManager::_updateParticles() { if (_currentEffect == EFFECT_NONE) return; for (auto &p : _particles) { p.x += p.speedX; p.y += p.speedY; if (_currentEffect == EFFECT_CLOUDS) { if (p.x > 360) p.x = -40; } else if (_currentEffect == EFFECT_SUN) { if (random(20) == 0) p.size = random(1, 4); } else { if (p.y > 240) { p.y = random(-50, -5); p.x = random(0, 320); } if (p.x > 320) p.x = 0; if (p.x < 0) p.x = 320; } } }
void UIManager::_drawParticles() { if (_currentEffect == EFFECT_NONE) return; for (auto &p : _particles) { switch (_currentEffect) { case EFFECT_SNOW: _mainSprite.fillCircle((int)p.x, (int)p.y, (int)p.size, p.color); break; case EFFECT_RAIN: _mainSprite.drawLine((int)p.x, (int)p.y, (int)p.x, (int)p.y + (int)p.size, p.color); break; case EFFECT_CONFETTI: _mainSprite.fillRect((int)p.x, (int)p.y, (int)p.size, (int)p.size, p.color); break; case EFFECT_SAKURA: _mainSprite.fillEllipse((int)p.x, (int)p.y, (int)p.size, (int)p.size-1, p.color); break; case EFFECT_CLOUDS: _mainSprite.fillCircle((int)p.x, (int)p.y, (int)p.size, p.color); _mainSprite.fillCircle((int)p.x - 5, (int)p.y - 5, (int)p.size-2, 0x528A); break; case EFFECT_SUN: _mainSprite.fillCircle((int)p.x, (int)p.y, (int)p.size, p.color); break; default: break; } } }