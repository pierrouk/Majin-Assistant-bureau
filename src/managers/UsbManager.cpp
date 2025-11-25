#include "UsbManager.h"

UsbManager::UsbManager() {
}

void UsbManager::begin() {
    Serial.println("🔌 [USB]: Démarrage HID Natif...");
    _mouse.begin();
    _keyboard.begin();
    _consumerControl.begin(); 
    USB.begin();
    _isReady = true;
    Serial.println("🔌 [USB]: Prêt.");
}

bool UsbManager::isConnected() { return _isReady; }

void UsbManager::moveMouse(int x, int y) { _mouse.move(x, y); }
void UsbManager::clickMouse() { _mouse.click(MOUSE_LEFT); }

// 🛠️ CORRECTION : Réintégration du mouvement manuel
void UsbManager::jiggleMouse() {
    if (!_isReady) return;
    Serial.println("🔌 [USB]: Jiggle manuel !");
    // Petit carré
    _mouse.move(10, 0); delay(50);
    _mouse.move(-10, 0); delay(50);
    _mouse.move(0, 10); delay(50);
    _mouse.move(0, -10);
}

// 🛠️ GESTION JIGGLER AUTOMATIQUE
void UsbManager::setJiggler(bool active) {
    _jigglerActive = active;
    Serial.printf("🔌 [USB]: Jiggler Auto %s\n", active ? "ACTIVÉ" : "DÉSACTIVÉ");
}

bool UsbManager::isJigglerActive() {
    return _jigglerActive;
}

void UsbManager::update() {
    if (!_jigglerActive || !_isReady) return;

    // Mouvement toutes les 50ms pour fluidité
    if (millis() - _lastJiggleTime > 50) {
        _lastJiggleTime = millis();
        
        int dist = 5;
        switch (_jiggleStep) {
            case 0: _mouse.move(dist, 0); break;
            case 1: _mouse.move(0, dist); break;
            case 2: _mouse.move(-dist, 0); break;
            case 3: _mouse.move(0, -dist); break;
        }
        
        _jiggleStep++;
        if (_jiggleStep > 3) _jiggleStep = 0;
    }
}

// --- STREAM DECK ---
void UsbManager::sendShortcut(uint8_t key, bool ctrl, bool shift, bool alt) {
    if (ctrl)  _keyboard.press(KEY_LEFT_CTRL);
    if (shift) _keyboard.press(KEY_LEFT_SHIFT);
    if (alt)   _keyboard.press(KEY_LEFT_ALT);
    _keyboard.press(key); delay(50); _keyboard.releaseAll();
}
void UsbManager::commandOBSStudio() { sendShortcut('r', true, true, false); }
void UsbManager::commandMute() { _consumerControl.press(CONSUMER_CONTROL_MUTE); delay(50); _consumerControl.release(); }
void UsbManager::commandCamera() { sendShortcut('v', true, true, false); }
void UsbManager::commandChat() { _keyboard.println("Bonjour depuis Majin OS (USB) !"); }