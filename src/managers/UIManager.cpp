#include "UIManager.h"
#include "../../include/Theme.h" 
#include <time.h> 

UIManager::UIManager() : _mainSprite(nullptr) {
}

void UIManager::begin(DisplayDriver* display, CoreManager* core, NetworkManager* net, SettingsManager* settings, UsbManager* usb) {
    _display = display; _core = core; _net = net; _settings = settings; _usb = usb;
    
    _mainSprite.setPsram(true); _mainSprite.setColorDepth(16); _mainSprite.createSprite(320, 240);
    _mainSprite.setFont(FONT_UI);

    // Init des sous-moteurs
    _face.begin(core, settings);
    _countdown.begin(core, settings); 
    _deck.begin(settings); 
    _overlay.begin(core, net, settings); // ⬅️ Init Overlay
    
    _initMenu();
    
    _currentScene = SCENE_FACE; 
}

void UIManager::_initMenu() {
    _menu.addApp("Souris", COLOR_PRIMARY, SCENE_APP_TRACKPAD);
    _menu.addApp("Deck",   COLOR_ACCENT,  SCENE_APP_STREAMDECK);
    _menu.addApp("Config", 0xFCA0,        SCENE_SETTINGS);
    _menu.addApp("System", COLOR_SUCCESS, SCENE_SETTINGS);
}

// --- DRAW LOOP PRINCIPALE ---
void UIManager::draw() {
    // 1. Fond (Sauf visage)
    if (_currentScene != SCENE_FACE) { _mainSprite.fillScreen(COLOR_BG); }

    // 2. Rendu de la Scène Active via Renderers
    switch (_currentScene) {
        case SCENE_FACE:           _face.draw(&_mainSprite); break;
        case SCENE_COUNTDOWN:      _countdown.draw(&_mainSprite); break;
        case SCENE_MENU:           _menu.draw(&_mainSprite); break;
        case SCENE_APP_STREAMDECK: _deck.draw(&_mainSprite, _currentDeckPage); break;
        
        case SCENE_APP_TRACKPAD:   _drawAppTrackpad(); break;     
        case SCENE_SETUP_WIFI:     _drawSetupWifi(); break;
        case SCENE_TAMA_MENU:      _drawTamaMenu(); break; 
        case SCENE_SETTINGS:       _mainSprite.drawString("REGLAGES (WIP)", 160, 120); break;
        default: break;
    }
    
    // 3. Effets Particules
    _drawParticles();

    // 4. OVERLAY & NOTIFICATIONS (Délégué)
    // L'Overlay gère maintenant : Barre Haut, Widgets Bas, Bulles de Notif
    // On l'affiche sur les écrans principaux
    if (_currentScene == SCENE_FACE || _currentScene == SCENE_COUNTDOWN || _currentScene == SCENE_APP_TRACKPAD) {
        _overlay.draw(&_mainSprite);
    }
    // Note : Si on veut les notifs sur TOUS les écrans, on peut appeler _overlay.draw() tout le temps,
    // mais attention à ne pas dessiner la barre météo sur le StreamDeck par exemple.
    // L'OverlayRenderer a une logique interne pour n'afficher les widgets que si nécessaire, 
    // mais ici on filtre pour être sûr.
    // Pour les notifs sur les autres écrans (Deck, Menu), on peut faire :
    else {
        // Astuce : On pourrait ajouter une methode drawOnlyNotification() dans OverlayRenderer
        // Pour l'instant, on laisse comme ça.
    }

    // 5. Status Souris (Resté ici car très spécifique USB)
    if (_usb->isJigglerActive() && _currentScene == SCENE_FACE) _drawMouseStatus();

    // 6. Push
    _display->secureDraw([&](LGFX_Majin* tft) {
        _mainSprite.pushSprite(tft, 0, 0);
    });
}

// --- UPDATE LOOP ---
void UIManager::update() {
    _updateParticles();
    
    // Mise à jour des animations
    if (_currentScene == SCENE_MENU) _menu.update(); 
    if (_currentScene == SCENE_FACE || _currentScene == SCENE_SETUP_WIFI) _face.update();

    // Mise à jour Overlay (pour le timer des notifications)
    _overlay.update();
}

// --- INTERACTIONS ---

// Wrapper pour la notification
void UIManager::showNotification(String message, uint32_t color, int durationMs) {
    _overlay.showNotification(message, color, durationMs);
}

bool UIManager::handleMenuClick(int touchX, int touchY) { 
    if (_currentScene != SCENE_MENU) return false; 
    
    UIScene target = _menu.handleClick(touchX, touchY);
    
    if (target != SCENE_MENU) {
        if (target == SCENE_APP_TRACKPAD && _menu.getSelectedAppName() == "Souris") {
             bool newState = !_usb->isJigglerActive(); 
             _usb->setJiggler(newState); 
             if (newState) showNotification("SOURIS AUTO", COLOR_PRIMARY, 2000); 
             else showNotification("ARRET SOURIS", 0xCE79, 2000); 
             setScene(SCENE_FACE); 
             return true;
        }
        setScene(target);
        return true;
    }
    return false; 
}

int UIManager::handleDeckClick(int touchX, int touchY) {
    if (_currentScene != SCENE_APP_STREAMDECK) return -1;
    int idx = _deck.handleClick(touchX, touchY, _currentDeckPage);
    if (idx != -1) {
         DeckButton btn = _settings->getDeckButton(idx);
         if(btn.active) showNotification("ACTION " + String(btn.label), btn.color, 1000);
    }
    return idx;
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

// --- EFFETS ---
void UIManager::setEffect(UIEffect effect) { if (_currentEffect != effect) { _currentEffect = effect; _initParticles(effect); } }
void UIManager::setWeatherEffect(UIEffect effect) { _weatherEffect = effect; if (_currentScene != SCENE_COUNTDOWN && _currentScene != SCENE_FACE) { setEffect(effect); } } 

void UIManager::setScene(UIScene scene) { 
    if (scene == SCENE_COUNTDOWN) { 
        int type = _settings->getEventType(); 
        switch(type) { 
            case 1: setEffect(EFFECT_CONFETTI); break; 
            case 2: setEffect(EFFECT_SNOW); break; 
            case 3: setEffect(EFFECT_SUN); break; 
            case 4: setEffect(EFFECT_SAKURA); break; 
            case 5: setEffect(EFFECT_SNOW); break; 
            default: setEffect(EFFECT_NONE); break; 
        } 
    } else { setEffect(EFFECT_NONE); } 
    _currentScene = scene; 
    if (scene == SCENE_COUNTDOWN) { _sceneStartTime = millis(); } 
}

UIScene UIManager::getScene() { return _currentScene; }

// --- DESSINS SCÈNES SECONDAIRES (Pas encore refactorés) ---

void UIManager::_drawAppTrackpad() { 
    _mainSprite.fillScreen(0x3333); _mainSprite.drawRect(10, 10, 300, 220, COLOR_WHITE); 
    _mainSprite.setTextColor(COLOR_WHITE); _mainSprite.setTextDatum(middle_center); _mainSprite.setFont(FONT_TITLE); _mainSprite.drawString("TRACKPAD", 160, 100); 
    _mainSprite.setFont(FONT_UI); _mainSprite.setTextColor(COLOR_PRIMARY); _mainSprite.drawString("Zone Active", 160, 140); 
    _mainSprite.setFont(FONT_SMALL); _mainSprite.setTextColor(0xCE79); _mainSprite.drawString("Appui Long pour quitter", 160, 200); 
}

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

void UIManager::_drawTamaMenu() { 
    _mainSprite.fillScreen(0x0804); _mainSprite.setTextDatum(top_center); _mainSprite.setTextColor(COLOR_WHITE); _mainSprite.setFont(FONT_TITLE); _mainSprite.drawString("SOINS", 160, 15); 
    int btnY = 140; int barY = 85; int barW = 50; int barH = 6; _mainSprite.setFont(FONT_SMALL); 
    int hunger = _core->getHunger(); int satFill = map(100 - hunger, 0, 100, 0, barW); _mainSprite.setTextColor(0xE463); _mainSprite.drawString("Faim", 60, barY - 15); _mainSprite.drawRect(60 - barW/2 - 1, barY - 1, barW + 2, barH + 2, COLOR_WHITE); _mainSprite.fillRect(60 - barW/2, barY, barW, barH, 0x2124); _mainSprite.fillRect(60 - barW/2, barY, satFill, barH, 0xE463); _mainSprite.fillCircle(60, btnY, 35, 0xE463); _mainSprite.setFont(FONT_UI); _mainSprite.setTextColor(COLOR_WHITE); _mainSprite.setTextDatum(middle_center); _mainSprite.drawString("Snack", 60, btnY + 50); int bx = 50; int by = btnY - 5; _mainSprite.fillRoundRect(bx, by, 20, 4, 2, 0xA145); _mainSprite.fillRect(bx, by+5, 20, 2, 0x2580); _mainSprite.fillRect(bx, by+8, 20, 3, 0x7000); _mainSprite.fillRoundRect(bx, by+12, 20, 4, 2, 0xA145); 
    int energy = _core->getEnergy(); int nrgFill = map(energy, 0, 100, 0, barW); uint16_t nrgCol = (energy < 30) ? COLOR_DANGER : COLOR_SUCCESS; _mainSprite.setFont(FONT_SMALL); _mainSprite.setTextColor(nrgCol); _mainSprite.setTextDatum(top_center); _mainSprite.drawString("Energie", 160, barY - 15); _mainSprite.drawRect(160 - barW/2 - 1, barY - 1, barW + 2, barH + 2, COLOR_WHITE); _mainSprite.fillRect(160 - barW/2, barY, barW, barH, 0x2124); _mainSprite.fillRect(160 - barW/2, barY, nrgFill, barH, nrgCol); uint16_t sleepCol = (_core->getMood() == MOOD_SLEEP) ? COLOR_SUCCESS : COLOR_PRIMARY; _mainSprite.fillCircle(160, btnY, 35, sleepCol); _mainSprite.setFont(FONT_UI); _mainSprite.setTextColor(COLOR_WHITE); _mainSprite.setTextDatum(middle_center); _mainSprite.drawString("Dodo", 160, btnY + 50); _mainSprite.drawString("Zzz", 160, btnY); 
    int fun = _core->getFun(); int funFill = map(fun, 0, 100, 0, barW); _mainSprite.setFont(FONT_SMALL); _mainSprite.setTextColor(COLOR_ACCENT); _mainSprite.setTextDatum(top_center); _mainSprite.drawString("Fun", 260, barY - 15); _mainSprite.drawRect(260 - barW/2 - 1, barY - 1, barW + 2, barH + 2, COLOR_WHITE); _mainSprite.fillRect(260 - barW/2, barY, barW, barH, 0x2124); _mainSprite.fillRect(260 - barW/2, barY, funFill, barH, COLOR_ACCENT); _mainSprite.fillCircle(260, btnY, 35, COLOR_ACCENT); _mainSprite.setFont(FONT_UI); _mainSprite.setTextColor(COLOR_WHITE); _mainSprite.setTextDatum(middle_center); _mainSprite.drawString("Jeu", 260, btnY + 50); _mainSprite.fillTriangle(255, btnY-10, 255, btnY+10, 270, btnY, COLOR_WHITE); 
    _mainSprite.setFont(FONT_SMALL); _mainSprite.setTextDatum(bottom_center); _mainSprite.setTextColor(0xCE79); _mainSprite.drawString("Swipe UP pour retour", 160, 235); 
}

void UIManager::_drawMouseStatus() { int x = 20; int y = 220; _mainSprite.fillRoundRect(x, y, 16, 22, 6, COLOR_PRIMARY); _mainSprite.drawLine(x+8, y, x+8, y+10, COLOR_BG); _mainSprite.fillRect(x+6, y+4, 4, 6, COLOR_BG); _mainSprite.drawArc(x+20, y, 6, 5, 30, 150, COLOR_PRIMARY); }

// --- PARTICULES ---
void UIManager::_initParticles(UIEffect type) { _particles.clear(); if (type == EFFECT_NONE) return; int count = MAX_PARTICLES; if (type == EFFECT_CLOUDS) count = 15; for (int i = 0; i < count; i++) { Particle p; p.x = random(0, 320); p.y = random(-240, 0); p.active = true; switch (type) { case EFFECT_SNOW: p.speedX = random(-10, 10)/10.0; p.speedY = random(10, 30)/10.0; p.size = random(1, 3); p.color = COLOR_WHITE; break; case EFFECT_RAIN: p.speedX = -0.5; p.speedY = random(40, 70)/10.0; p.size = random(5, 10); p.color = 0x001F; break; case EFFECT_CONFETTI: p.speedX = random(-20, 20)/10.0; p.speedY = random(20, 50)/10.0; p.size = random(2, 5); p.color = (random(2))?COLOR_PRIMARY:(random(2)?COLOR_ACCENT:COLOR_SUCCESS); if(random(10)>8) p.color = 0xFFE0; break; case EFFECT_SAKURA: p.speedX = random(-15, 15)/10.0; p.speedY = random(10, 25)/10.0; p.size = random(2, 4); p.color = 0xF63D; break; case EFFECT_CLOUDS: p.y = random(0, 100); p.speedX = random(2, 8) / 10.0; p.speedY = 0; p.size = random(20, 40); p.color = 0x3186; break; case EFFECT_SUN: p.x = random(0, 320); p.y = random(0, 240); p.speedX = 0; p.speedY = 0; p.size = random(1, 3); p.color = 0xFFE0; break; default: break; } _particles.push_back(p); } }
void UIManager::_updateParticles() { if (_currentEffect == EFFECT_NONE) return; for (auto &p : _particles) { p.x += p.speedX; p.y += p.speedY; if (_currentEffect == EFFECT_CLOUDS) { if (p.x > 360) p.x = -40; } else if (_currentEffect == EFFECT_SUN) { if (random(20) == 0) p.size = random(1, 4); } else { if (p.y > 240) { p.y = random(-50, -5); p.x = random(0, 320); } if (p.x > 320) p.x = 0; if (p.x < 0) p.x = 320; } } }
void UIManager::_drawParticles() { if (_currentEffect == EFFECT_NONE) return; for (auto &p : _particles) { switch (_currentEffect) { case EFFECT_SNOW: _mainSprite.fillCircle((int)p.x, (int)p.y, (int)p.size, p.color); break; case EFFECT_RAIN: _mainSprite.drawLine((int)p.x, (int)p.y, (int)p.x, (int)p.y + (int)p.size, p.color); break; case EFFECT_CONFETTI: _mainSprite.fillRect((int)p.x, (int)p.y, (int)p.size, (int)p.size, p.color); break; case EFFECT_SAKURA: _mainSprite.fillEllipse((int)p.x, (int)p.y, (int)p.size, (int)p.size-1, p.color); break; case EFFECT_CLOUDS: _mainSprite.fillCircle((int)p.x, (int)p.y, (int)p.size, p.color); _mainSprite.fillCircle((int)p.x - 5, (int)p.y - 5, (int)p.size-2, 0x528A); break; case EFFECT_SUN: _mainSprite.fillCircle((int)p.x, (int)p.y, (int)p.size, p.color); break; default: break; } } }