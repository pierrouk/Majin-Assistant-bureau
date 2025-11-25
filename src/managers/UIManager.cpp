#include "UIManager.h"
#include "../../include/Theme.h" 

// 🛠️ CORRECTIF : Constructeur nettoyé (plus de _pixelSprite)
UIManager::UIManager() : _mainSprite(nullptr) {
}

void UIManager::begin(DisplayDriver* display, CoreManager* core, NetworkManager* net, SettingsManager* settings, UsbManager* usb) {
    _display = display; _core = core; _net = net; _settings = settings; _usb = usb;
    
    _mainSprite.setPsram(true); _mainSprite.setColorDepth(16); _mainSprite.createSprite(320, 240);
    _mainSprite.setFont(FONT_UI);

    // Init Renderers
    _face.begin(core, settings);
    _countdown.begin(core, settings); // ⬅️ Init

    _initApps();
    _currentScene = SCENE_FACE; 
}

void UIManager::_initApps() {
    _apps.push_back({"Souris", COLOR_PRIMARY, 80,  100, 30, SCENE_APP_TRACKPAD});
    _apps.push_back({"Deck",   COLOR_ACCENT,  240, 100, 30, SCENE_APP_STREAMDECK});
    _apps.push_back({"Config", 0xFCA0,        80,  200, 30, SCENE_SETTINGS}); 
    _apps.push_back({"System", COLOR_SUCCESS, 240, 200, 30, SCENE_SETTINGS});
}

// --- HELPER ICONES ---
void _drawDeckIcon(LGFX_Sprite* spr, int id, int x, int y, uint16_t color) {
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

// --- EFFETS & PARTICULES ---
void UIManager::setEffect(UIEffect effect) { if (_currentEffect != effect) { _currentEffect = effect; _initParticles(effect); } }
void UIManager::setWeatherEffect(UIEffect effect) { _weatherEffect = effect; if (_currentScene != SCENE_COUNTDOWN) { setEffect(effect); } }

void UIManager::_initParticles(UIEffect type) { 
    _particles.clear(); 
    if (type == EFFECT_NONE) return; 
    int count = MAX_PARTICLES;
    if (type == EFFECT_CLOUDS) count = 15; 

    for (int i = 0; i < count; i++) { 
        Particle p; 
        p.x = random(0, 320); p.y = random(-240, 0); p.active = true; 
        switch (type) { 
            case EFFECT_SNOW: p.speedX = random(-10, 10)/10.0; p.speedY = random(10, 30)/10.0; p.size = random(1, 3); p.color = COLOR_WHITE; break; 
            case EFFECT_RAIN: p.speedX = -0.5; p.speedY = random(40, 70)/10.0; p.size = random(5, 10); p.color = 0x001F; break; 
            case EFFECT_CONFETTI: p.speedX = random(-20, 20)/10.0; p.speedY = random(20, 50)/10.0; p.size = random(2, 5); p.color = (random(2))?COLOR_PRIMARY:(random(2)?COLOR_ACCENT:COLOR_SUCCESS); if(random(10)>8) p.color = 0xFFE0; break; 
            case EFFECT_SAKURA: p.speedX = random(-15, 15)/10.0; p.speedY = random(10, 25)/10.0; p.size = random(2, 4); p.color = 0xF63D; break; 
            case EFFECT_CLOUDS: p.y = random(0, 100); p.speedX = random(2, 8) / 10.0; p.speedY = 0; p.size = random(20, 40); p.color = 0x3186; break; 
            case EFFECT_SUN: p.x = random(0, 320); p.y = random(0, 240); p.speedX = 0; p.speedY = 0; p.size = random(1, 3); p.color = 0xFFE0; break; 
            default: break; 
        } 
        _particles.push_back(p); 
    } 
}

void UIManager::_updateParticles() { 
    if (_currentEffect == EFFECT_NONE) return; 
    for (auto &p : _particles) { 
        p.x += p.speedX; p.y += p.speedY; 
        if (_currentEffect == EFFECT_CLOUDS) { 
            if (p.x > 360) p.x = -40; 
        } else if (_currentEffect == EFFECT_SUN) { 
            if (random(20) == 0) p.size = random(1, 4); 
        } else { 
            if (p.y > 240) { p.y = random(-50, -5); p.x = random(0, 320); } 
            if (p.x > 320) p.x = 0; if (p.x < 0) p.x = 320; 
        } 
    } 
}

void UIManager::_drawOverlay() { 
    if (_currentEffect == EFFECT_NONE) return; 
    for (auto &p : _particles) { 
        switch (_currentEffect) { 
            case EFFECT_SNOW: _mainSprite.fillCircle((int)p.x, (int)p.y, (int)p.size, p.color); break; 
            case EFFECT_RAIN: _mainSprite.drawLine((int)p.x, (int)p.y, (int)p.x, (int)p.y + (int)p.size, p.color); break; 
            case EFFECT_CONFETTI: _mainSprite.fillRect((int)p.x, (int)p.y, (int)p.size, (int)p.size, p.color); break; 
            case EFFECT_SAKURA: _mainSprite.fillEllipse((int)p.x, (int)p.y, (int)p.size, (int)p.size-1, p.color); break; 
            case EFFECT_CLOUDS: _mainSprite.fillCircle((int)p.x, (int)p.y, (int)p.size, p.color); _mainSprite.fillCircle((int)p.x - 5, (int)p.y - 5, (int)p.size-2, 0x528A); break; 
            case EFFECT_SUN: _mainSprite.fillCircle((int)p.x, (int)p.y, (int)p.size, p.color); break; 
            default: break; 
        } 
    } 
}

// --- INTERACTIONS ---
bool UIManager::handleMenuClick(int touchX, int touchY) { 
    if (_currentScene != SCENE_MENU) return false; 
    for (auto &app : _apps) { 
        int dx = touchX - app.x; int dy = touchY - app.y; 
        if ((dx*dx + dy*dy) <= (app.radius * app.radius)) { 
            if (app.name == "Souris") { bool newState = !_usb->isJigglerActive(); _usb->setJiggler(newState); if (newState) showNotification("SOURIS AUTO: ACTIVEE", COLOR_PRIMARY, 2000); else showNotification("SOURIS AUTO: ARRET", 0xCE79, 2000); setScene(SCENE_FACE); return true; } 
            setScene(app.targetScene); return true; 
        } 
    } 
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

// --- UPDATE ---
void UIManager::update() {
    _updateParticles();

    // 🛠️ CORRECTIF : On délègue la mise à jour du visage au FaceRenderer
    if (_currentScene == SCENE_FACE || _currentScene == SCENE_SETUP_WIFI) {
        _face.update();
    }
    
    if (_currentScene == SCENE_COUNTDOWN) { if (millis() - _sceneStartTime > SCENE_TIMEOUT) setScene(SCENE_FACE); }
    if (_notifDuration > 0 && (millis() - _notifStartTime > _notifDuration)) { _notifMsg = ""; _notifDuration = 0; }
}

// --- DRAW ---
void UIManager::draw() {
    if (_currentScene != SCENE_FACE) { _mainSprite.fillScreen(COLOR_BG); }

    switch (_currentScene) {
        case SCENE_FACE: _face.draw(&_mainSprite); break;
        
        // ⚠️ DÉLÉGATION
        case SCENE_COUNTDOWN: _countdown.draw(&_mainSprite); break;
        
        case SCENE_MENU: _drawSceneMenu(); break;
        case SCENE_APP_TRACKPAD: _drawAppTrackpad(); break;     
        case SCENE_APP_STREAMDECK: _drawAppStreamDeck(); break;
        case SCENE_SETUP_WIFI: _drawSetupWifi(); break;
        case SCENE_TAMA_MENU: _drawTamaMenu(); break; 
        case SCENE_SETTINGS: _mainSprite.drawString("REGLAGES (WIP)", 160, 120); break;
        default: break;
    }
    
    // ... (Overlay et Notifications inchangés) ...
    if (_currentScene == SCENE_FACE || _currentScene == SCENE_COUNTDOWN) {
        _drawOverlay();
    }

    if (_currentScene != SCENE_COUNTDOWN) _drawWidgets();
    if (_notifMsg != "") _drawNotification();
    if (_usb->isJigglerActive() && _currentScene == SCENE_FACE) _drawMouseStatus();

    _display->secureDraw([&](LGFX_Majin* tft) {
        _mainSprite.pushSprite(tft, 0, 0);
    });
}

// --- DESSINS INTERFACE ---

void UIManager::_drawAppStreamDeck() {
    _mainSprite.fillScreen(COLOR_BG); 
    _mainSprite.setTextColor(COLOR_WHITE); 
    _mainSprite.setFont(FONT_TITLE); 
    _mainSprite.drawString("STREAM DECK", 160, 20); 
    _mainSprite.setFont(FONT_UI); 

    int startX = 25; int startY = 50; int btnW = 80; int btnH = 60; int gap = 15;
    int startIndex = _currentDeckPage * 6; 

    for(int i=0; i<6; i++) {
        int col = i % 3; int row = i / 3;
        int x = startX + col * (btnW + gap); int y = startY + row * (btnH + gap);
        
        DeckButton btn = _settings->getDeckButton(startIndex + i);
        
        if (btn.active) {
            _mainSprite.fillRoundRect(x, y, btnW, btnH, 8, btn.color);
            _mainSprite.drawRoundRect(x, y, btnW, btnH, 8, COLOR_WHITE);
            
            if (btn.iconID > 0) {
                _drawDeckIcon(&_mainSprite, btn.iconID, x + btnW/2, y + btnH/2 - 5, COLOR_BLACK);
                _mainSprite.setTextDatum(bottom_center);
                _mainSprite.setTextColor(COLOR_BLACK);
                _mainSprite.setFont(FONT_SMALL); 
                _mainSprite.drawString(btn.label, x + btnW/2, y + btnH - 5);
            } else {
                _mainSprite.setTextDatum(middle_center);
                _mainSprite.setTextColor(COLOR_BLACK);
                _mainSprite.setFont(FONT_UI); 
                _mainSprite.drawString(btn.label, x + btnW/2, y + btnH/2);
            }
        } else {
            _mainSprite.drawRoundRect(x, y, btnW, btnH, 8, 0x3333); 
            _mainSprite.setTextColor(0x5555);
            _mainSprite.setTextDatum(middle_center);
            _mainSprite.drawString("+", x + btnW/2, y + btnH/2);
        }
    }
    
    int dotY = 200;
    for(int p=0; p<4; p++) {
        uint16_t color = (p == _currentDeckPage) ? COLOR_PRIMARY : 0x3333;
        _mainSprite.fillCircle(160 - 30 + (p*20), dotY, 4, color);
    }
    _mainSprite.setFont(FONT_SMALL); 
    _mainSprite.setTextColor(0xCE79);
    _mainSprite.setTextDatum(bottom_center); 
    _mainSprite.drawString("Swipe < >", 160, 230); 
}

void UIManager::_drawMouseStatus() { int x = 20; int y = 220; _mainSprite.fillRoundRect(x, y, 16, 22, 6, COLOR_PRIMARY); _mainSprite.drawLine(x+8, y, x+8, y+10, COLOR_BG); _mainSprite.fillRect(x+6, y+4, 4, 6, COLOR_BG); _mainSprite.drawArc(x+20, y, 6, 5, 30, 150, COLOR_PRIMARY); }

void UIManager::_drawTamaMenu() { 
    _mainSprite.fillScreen(0x0804); 
    _mainSprite.setTextDatum(top_center); _mainSprite.setTextColor(COLOR_WHITE); _mainSprite.setFont(FONT_TITLE); _mainSprite.drawString("SOINS", 160, 15); 
    int btnY = 140; int barY = 85; int barW = 50; int barH = 6; 
    _mainSprite.setFont(FONT_SMALL); 
    
    // Faim
    int hunger = _core->getHunger(); int satFill = map(100 - hunger, 0, 100, 0, barW); 
    _mainSprite.setTextColor(0xE463); _mainSprite.drawString("Faim", 60, barY - 15); 
    _mainSprite.drawRect(60 - barW/2 - 1, barY - 1, barW + 2, barH + 2, COLOR_WHITE); _mainSprite.fillRect(60 - barW/2, barY, barW, barH, 0x2124); _mainSprite.fillRect(60 - barW/2, barY, satFill, barH, 0xE463); 
    _mainSprite.fillCircle(60, btnY, 35, 0xE463); _mainSprite.setFont(FONT_UI); _mainSprite.setTextColor(COLOR_WHITE); _mainSprite.setTextDatum(middle_center); _mainSprite.drawString("Snack", 60, btnY + 50); 
    int bx = 50; int by = btnY - 5; _mainSprite.fillRoundRect(bx, by, 20, 4, 2, 0xA145); _mainSprite.fillRect(bx, by+5, 20, 2, 0x2580); _mainSprite.fillRect(bx, by+8, 20, 3, 0x7000); _mainSprite.fillRoundRect(bx, by+12, 20, 4, 2, 0xA145); 

    // Energie
    int energy = _core->getEnergy(); int nrgFill = map(energy, 0, 100, 0, barW); uint16_t nrgCol = (energy < 30) ? COLOR_DANGER : COLOR_SUCCESS; 
    _mainSprite.setFont(FONT_SMALL); _mainSprite.setTextColor(nrgCol); _mainSprite.setTextDatum(top_center); _mainSprite.drawString("Energie", 160, barY - 15); 
    _mainSprite.drawRect(160 - barW/2 - 1, barY - 1, barW + 2, barH + 2, COLOR_WHITE); _mainSprite.fillRect(160 - barW/2, barY, barW, barH, 0x2124); _mainSprite.fillRect(160 - barW/2, barY, nrgFill, barH, nrgCol); 
    uint16_t sleepCol = (_core->getMood() == MOOD_SLEEP) ? COLOR_SUCCESS : COLOR_PRIMARY; 
    _mainSprite.fillCircle(160, btnY, 35, sleepCol); _mainSprite.setFont(FONT_UI); _mainSprite.setTextColor(COLOR_WHITE); _mainSprite.setTextDatum(middle_center); _mainSprite.drawString("Dodo", 160, btnY + 50); _mainSprite.drawString("Zzz", 160, btnY); 

    // Fun
    int fun = _core->getFun(); int funFill = map(fun, 0, 100, 0, barW); 
    _mainSprite.setFont(FONT_SMALL); _mainSprite.setTextColor(COLOR_ACCENT); _mainSprite.setTextDatum(top_center); _mainSprite.drawString("Fun", 260, barY - 15); 
    _mainSprite.drawRect(260 - barW/2 - 1, barY - 1, barW + 2, barH + 2, COLOR_WHITE); _mainSprite.fillRect(260 - barW/2, barY, barW, barH, 0x2124); _mainSprite.fillRect(260 - barW/2, barY, funFill, barH, COLOR_ACCENT); 
    _mainSprite.fillCircle(260, btnY, 35, COLOR_ACCENT); _mainSprite.setFont(FONT_UI); _mainSprite.setTextColor(COLOR_WHITE); _mainSprite.setTextDatum(middle_center); _mainSprite.drawString("Jeu", 260, btnY + 50); _mainSprite.fillTriangle(255, btnY-10, 255, btnY+10, 270, btnY, COLOR_WHITE); 

    _mainSprite.setFont(FONT_SMALL); _mainSprite.setTextDatum(bottom_center); _mainSprite.setTextColor(0xCE79); _mainSprite.drawString("Swipe UP pour retour", 160, 235); 
}

void UIManager::_drawSetupWifi() { 
    _mainSprite.fillScreen(COLOR_UI_BG); 
    
    if (_settings->isSetupDone()) {
        // --- CAS 1 : ROBOT CONNECTÉ (Mode Infos) ---
        // On affiche l'IP et le réseau pour info
        
        _mainSprite.setTextColor(COLOR_WHITE); 
        _mainSprite.setTextDatum(top_center); 
        _mainSprite.setFont(FONT_TITLE); 
        _mainSprite.drawString("INFOS RESEAU", 160, 20); 

        // Cadre stylé
        _mainSprite.drawRoundRect(20, 60, 280, 100, 8, COLOR_PRIMARY);
        
        _mainSprite.setFont(FONT_UI);
        _mainSprite.setTextDatum(middle_center);
        
        // Récupération dynamique du SSID (Nom du WiFi) et de l'IP
        String ssid = WiFi.SSID();
        String ip = _net->getIP();
        
        // Affichage
        _mainSprite.setTextColor(COLOR_ACCENT);
        _mainSprite.drawString("WiFi: " + ssid, 160, 90);
        
        _mainSprite.setTextColor(COLOR_SUCCESS);
        _mainSprite.drawString("IP: " + ip, 160, 120);
        
        // Instructions de sortie
        _mainSprite.setFont(FONT_SMALL);
        _mainSprite.setTextColor(0xCE79);
        _mainSprite.setTextDatum(bottom_center);
        _mainSprite.drawString("Swipe ou Tap pour fermer", 160, 220);
        
    } else {
        // --- CAS 2 : ROBOT NON CONFIGURÉ (Mode AP) ---
        // Instructions de configuration d'origine
        
        _mainSprite.fillRoundRect(160 - 20, 60, 15, 40, 5, COLOR_WHITE); 
        _mainSprite.fillRoundRect(160 + 5, 60, 15, 40, 5, COLOR_WHITE); 
        
        _mainSprite.setTextColor(COLOR_WHITE); 
        _mainSprite.setTextDatum(top_center); 
        _mainSprite.setFont(FONT_TITLE); 
        _mainSprite.drawString("CONFIGURATION", 160, 110); 
        
        _mainSprite.setFont(FONT_SMALL); 
        _mainSprite.setTextColor(COLOR_PRIMARY); 
        _mainSprite.drawString("1. Connectez-vous au WiFi:", 160, 150); 
        
        _mainSprite.setFont(FONT_TITLE); 
        _mainSprite.setTextColor(COLOR_ACCENT); 
        _mainSprite.drawString("Majin_Setup", 160, 175); 
        
        _mainSprite.setFont(FONT_SMALL); 
        _mainSprite.setTextColor(COLOR_WHITE); 
        _mainSprite.drawString("2. Configurez le réseau", 160, 210); 
    }
}

void UIManager::_drawSceneMenu() { 
    _mainSprite.fillScreen(COLOR_UI_BG); _mainSprite.setTextDatum(top_center); _mainSprite.setTextColor(COLOR_WHITE); _mainSprite.setFont(FONT_TITLE); _mainSprite.drawString("MENU", 160, 10); 
    _mainSprite.setFont(FONT_SMALL); for (auto &app : _apps) { _mainSprite.fillCircle(app.x, app.y, app.radius, app.color); _mainSprite.setTextDatum(top_center); _mainSprite.drawString(app.name, app.x, app.y + app.radius + 8); } 
}

void UIManager::_drawAppTrackpad() { 
    _mainSprite.fillScreen(0x3333); _mainSprite.drawRect(10, 10, 300, 220, COLOR_WHITE); 
    _mainSprite.setTextColor(COLOR_WHITE); _mainSprite.setTextDatum(middle_center); _mainSprite.setFont(FONT_TITLE); _mainSprite.drawString("TRACKPAD", 160, 100); 
    _mainSprite.setFont(FONT_UI); _mainSprite.setTextColor(COLOR_PRIMARY); _mainSprite.drawString("Zone Active", 160, 140); 
    _mainSprite.setFont(FONT_SMALL); _mainSprite.setTextColor(0xCE79); _mainSprite.drawString("Appui Long pour quitter", 160, 200); 
}

void UIManager::_drawWidgets() { 
    if (_settings->isSetupDone()) { 
        if (_settings->getShowTime()) { 
            _mainSprite.setFont(FONT_TITLE); _mainSprite.setTextColor(COLOR_WHITE); _mainSprite.setTextDatum(top_right); 
            _mainSprite.drawString(_core->getTimeString().substring(0, 5), 310, 5); 
        } 
        if (_settings->getShowSensors()) { 
            _mainSprite.setFont(FONT_SMALL); _mainSprite.setTextDatum(top_left); 
            String tempStr = String(_core->getTemp(), 1) + "C"; String humStr  = String(_core->getHum(), 0) + "%"; 
            _mainSprite.setTextColor(COLOR_PRIMARY); _mainSprite.drawString(tempStr, 5, 5); 
            _mainSprite.setTextColor(COLOR_PRIMARY); _mainSprite.drawString(humStr, 5, 22); 
        } 
    } 
}

void UIManager::_drawNotification() { 
    _mainSprite.fillRect(0, 0, 320, 50, _notifColor); _mainSprite.fillRect(0, 48, 320, 2, COLOR_WHITE); 
    _mainSprite.setFont(FONT_TITLE); _mainSprite.setTextColor(COLOR_WHITE); _mainSprite.setTextDatum(middle_center); _mainSprite.drawString(_notifMsg, 160, 25); 
}



void UIManager::setScene(UIScene scene) { if (scene == SCENE_COUNTDOWN) { int type = _settings->getEventType(); switch(type) { case 0: setEffect(EFFECT_NONE); break; case 1: setEffect(EFFECT_CONFETTI); break; case 2: setEffect(EFFECT_SNOW); break; case 3: setEffect(EFFECT_SAKURA); break; default: setEffect(EFFECT_NONE); break; } } else { setEffect(_weatherEffect); } _currentScene = scene; if (scene == SCENE_COUNTDOWN) { _sceneStartTime = millis(); } }
UIScene UIManager::getScene() { return _currentScene; }
void UIManager::showNotification(String message, uint32_t color, int durationMs) { _notifMsg = message; _notifColor = color; _notifDuration = durationMs; _notifStartTime = millis(); }