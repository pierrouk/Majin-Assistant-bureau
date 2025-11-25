#include "CoreManager.h"
#include "../../include/Theme.h" 

CoreManager::CoreManager() {
}

void CoreManager::begin(SettingsManager* settings) {
    _settings = settings;
    configTime(GMT_OFFSET_SEC, DAYLIGHT_OFFSET_SEC, NTP_SERVER);
    
    _energy = settings->getStatEnergy();
    _hunger = settings->getStatHunger();
    _fun    = settings->getStatFun();
    
    Serial.printf("🥚 [Core]: Stats chargées -> E:%d H:%d F:%d\n", (int)_energy, (int)_hunger, (int)_fun);
}

void CoreManager::update() {
    _decayStats();
    _calculateMood();
}

// 🧬 MOTEUR BIOLOGIQUE RÉALISTE
void CoreManager::_decayStats() {
    static unsigned long lastDecay = 0;
    static unsigned long lastSave = 0;
    
    // Mise à jour toutes les MINUTES (60s) au lieu de 10s
    // Pour un rythme de vie lent et réaliste
    if (millis() - lastDecay > 60000) { 
        lastDecay = millis();

        // Récupération de l'heure réelle
        struct tm timeinfo;
        bool timeValid = getLocalTime(&timeinfo);
        int hour = timeValid ? timeinfo.tm_hour : 12; // 12h par défaut si pas de wifi

        // --- 1. GESTION ÉNERGIE (Cycle 16h Éveil / 8h Sommeil) ---
        if (_isSleeping) {
            // Récupère ~12.5% par heure -> 8h pour 0 à 100%
            _energy += 0.21; 
            
            // Réveil automatique le matin (après 7h) si énergie pleine
            if (_energy >= 100 && hour >= 7 && hour < 10) {
                wakeUp();
            }
        } else {
            // Perte ~6.25% par heure -> 16h d'autonomie
            float dropRate = 0.11;
            
            // Fatigue accélérée la nuit (après 23h)
            if (hour >= 23 || hour < 6) dropRate = 0.25;
            
            _energy -= dropRate;
        }

        // --- 2. GESTION FAIM (Pics aux repas) ---
        if (!_isSleeping) {
            float hungerRate = 0.05; // Base très lente (3% par heure)
            
            // Pic Midi (11h-14h)
            if (hour >= 11 && hour <= 13) hungerRate = 0.4; // ~24% par heure
            
            // Pic Soir (18h-21h)
            if (hour >= 18 && hour <= 20) hungerRate = 0.4;
            
            _hunger += hungerRate;
        }

        // --- 3. GESTION FUN (Distraction) ---
        if (!_isSleeping) {
            // Baisse constante : ~12% par heure (Besoin de jouer toutes les 4-5h)
            _fun -= 0.2;
        }

        // Bornes de sécurité (Clamping)
        if (_energy > 100) _energy = 100; if (_energy < 0) _energy = 0;
        if (_hunger > 100) _hunger = 100; if (_hunger < 0) _hunger = 0;
        if (_fun > 100)    _fun = 100;    if (_fun < 0)    _fun = 0;

        // Debug Log (toutes les minutes)
        // Serial.printf("🥚 [Bio]: E:%.1f H:%.1f F:%.1f (Heure: %d)\n", _energy, _hunger, _fun, hour);
    }

    // Sauvegarde NVS toutes les 10 minutes (pour préserver la mémoire flash)
    if (millis() - lastSave > 600000) {
        lastSave = millis();
        _saveStats();
    }
}

void CoreManager::_saveStats() {
    _settings->setStats((int)_energy, (int)_hunger, (int)_fun);
}

void CoreManager::_calculateMood() {
    if (_isSleeping) { _currentMood = MOOD_SLEEP; return; }
    
    // Hiérarchie des besoins ajustée
    if (_energy < 10) { _currentMood = MOOD_TIRED; return; } // Épuisé
    if (_hunger > 75) { _currentMood = MOOD_HUNGRY; return; } // A faim
    if (_fun < 15)    { _currentMood = MOOD_BORED; return; }  // S'ennuie

    // État par défaut
    if (_fun > 70 && _energy > 40 && _hunger < 40) {
        _currentMood = MOOD_HAPPY;
    } else {
        _currentMood = MOOD_NEUTRAL;
    }
}

void CoreManager::setMood(MoodState newMood) {
    _currentMood = newMood;
    // Cheat Mode : Force les stats pour matcher l'humeur demandée
    if (newMood == MOOD_HAPPY) { _fun = 100; _hunger = 0; _energy = 100; _isSleeping = false; }
    else if (newMood == MOOD_ANGRY) { _fun = 0; _hunger = 90; }
    else if (newMood == MOOD_NEUTRAL) { _fun = 50; _hunger = 50; _energy = 50; }
    _saveStats();
}

void CoreManager::resetLife() {
    _energy = 100; _hunger = 0; _fun = 100; _isSleeping = false;
    _currentMood = MOOD_HAPPY;
    _saveStats();
    Serial.println("🥚 [Core]: Reset Vie Complet !");
}

// --- ACTIONS ---

void CoreManager::feed(int amount) {
    _hunger -= amount;
    if (_hunger < 0) _hunger = 0;
    _fun += 2; // Manger fait un peu plaisir
    _saveStats(); // Sauvegarde immédiate pour valider l'action
}

void CoreManager::play(int amount) {
    if (_isSleeping) return; 
    _fun += amount;
    if (_fun > 100) _fun = 100;
    _energy -= 2; // Jouer fatigue un peu
    _hunger += 2; // Et creuse l'appétit
    _saveStats();
}

void CoreManager::sleep(bool force) {
    _isSleeping = true;
    _currentMood = MOOD_SLEEP;
    _saveStats();
}

void CoreManager::wakeUp() {
    _isSleeping = false;
    _calculateMood();
    _saveStats();
}

void CoreManager::changeEnergy(int delta) {
    _energy += delta;
    if (_energy > 100) _energy = 100; if (_energy < 0) _energy = 0;
    _saveStats();
}

uint16_t CoreManager::getGlobalColor() {
    // Formule de santé globale
    int health = (_energy + _fun + (100 - _hunger)) / 3;
    if (health > 75) return COLOR_SUCCESS; 
    if (health > 35) return 0xFFE0;        
    return COLOR_DANGER;                   
}

String CoreManager::getTimeString() {
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo)) return "--:--";
    char timeString[6];
    strftime(timeString, sizeof(timeString), "%H:%M", &timeinfo);
    return String(timeString);
}

MoodState CoreManager::getMood() { return _currentMood; }
void CoreManager::setSensorData(float temp, float hum, float lux) { _temp = temp; _hum = hum; _lux = lux; }
float CoreManager::getTemp() { return _temp; }
float CoreManager::getHum()  { return _hum; }
float CoreManager::getLux()  { return _lux; }
int CoreManager::getEnergy() { return (int)_energy; }
int CoreManager::getHunger() { return (int)_hunger; }
int CoreManager::getFun()    { return (int)_fun; }