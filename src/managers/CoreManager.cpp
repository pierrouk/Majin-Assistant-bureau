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
    _checkAutoSleep(); // ⬅️ Vérification Lumière
    _calculateMood();
}

// 💡 LOGIQUE SOMMEIL AUTOMATIQUE (DYNAMIQUE)
void CoreManager::_checkAutoSleep() {
    // Récupération du seuil configuré (ex: 2.0 Lux)
    float threshold = _settings->getAutoSleepThreshold();
    
    // SEUIL NUIT (Si < seuil)
    if (_lux < threshold) {
        if (!_isSleeping) {
            // Si le timer n'est pas lancé, on le lance
            if (_darknessTimer == 0) {
                _darknessTimer = millis();
            }
            // Si cela fait plus de 5 secondes qu'il fait noir
            else if (millis() - _darknessTimer > 5000) {
                Serial.println("🌑 [Core]: Il fait noir... Dodo zzz");
                sleep(false); // false = pas forcé par user, mais par environnement
                _darknessTimer = 0; // Reset
            }
        }
    } 
    // SEUIL JOUR (Si > seuil + 5 pour hystérésis)
    else if (_lux > (threshold + 5.0)) {
        _darknessTimer = 0; // On reset le timer si la lumière revient
        
        if (_isSleeping) {
            Serial.println("☀️ [Core]: Lumière détectée ! Réveil !");
            wakeUp();
        }
    }
}

void CoreManager::_decayStats() {
    static unsigned long lastDecay = 0;
    static unsigned long lastSave = 0;
    if (millis() - lastDecay > 60000) { 
        lastDecay = millis();
        struct tm timeinfo; bool timeValid = getLocalTime(&timeinfo); int hour = timeValid ? timeinfo.tm_hour : 12;
        if (_isSleeping) { _energy += 0.21; } // On enlève le réveil horaire ici, c'est la lumière qui décide
        else { float dropRate = 0.11; if (hour >= 23 || hour < 6) dropRate = 0.25; _energy -= dropRate; }
        if (!_isSleeping) { float hungerRate = 0.05; if (hour >= 11 && hour <= 13) hungerRate = 0.4; if (hour >= 18 && hour <= 20) hungerRate = 0.4; _hunger += hungerRate; _fun -= 0.2; }
        if (_energy > 100) _energy = 100; if (_energy < 0) _energy = 0;
        if (_hunger > 100) _hunger = 100; if (_hunger < 0) _hunger = 0;
        if (_fun > 100)    _fun = 100;    if (_fun < 0)    _fun = 0;
    }
    if (millis() - lastSave > 600000) { lastSave = millis(); _saveStats(); }
}

void CoreManager::_calculateMood() {
    int manualMood = _settings->getManualMood();
    if (manualMood != -1) { _currentMood = (MoodState)manualMood; return; }

    if (_isSleeping) { _currentMood = MOOD_SLEEP; return; }
    if (_energy < 10) { _currentMood = MOOD_TIRED; return; } 
    if (_hunger > 75) { _currentMood = MOOD_HUNGRY; return; } 
    if (_fun < 15)    { _currentMood = MOOD_BORED; return; }  

    if (_fun > 70 && _energy > 40 && _hunger < 40) { _currentMood = MOOD_HAPPY; } 
    else { _currentMood = MOOD_NEUTRAL; }
}

void CoreManager::_saveStats() { _settings->setStats((int)_energy, (int)_hunger, (int)_fun); }
void CoreManager::setMood(MoodState newMood) {
    _currentMood = newMood;
    if (newMood == MOOD_HAPPY) { _fun = 100; _hunger = 0; _energy = 100; _isSleeping = false; }
    else if (newMood == MOOD_ANGRY) { _fun = 0; _hunger = 90; }
    else if (newMood == MOOD_NEUTRAL) { _fun = 50; _hunger = 50; _energy = 50; }
    _saveStats();
}
void CoreManager::resetLife() { _energy = 100; _hunger = 0; _fun = 100; _isSleeping = false; _currentMood = MOOD_HAPPY; _saveStats(); Serial.println("🥚 [Core]: Reset Vie Complet !"); }
void CoreManager::feed(int amount) { _hunger -= amount; if (_hunger < 0) _hunger = 0; _fun += 2; _saveStats(); }
void CoreManager::play(int amount) { if (_isSleeping) return; _fun += amount; if (_fun > 100) _fun = 100; _energy -= 2; _hunger += 2; _saveStats(); }
void CoreManager::sleep(bool force) { _isSleeping = true; _currentMood = MOOD_SLEEP; _saveStats(); }
void CoreManager::wakeUp() { _isSleeping = false; _calculateMood(); _saveStats(); }
void CoreManager::changeEnergy(int delta) { _energy += delta; if (_energy > 100) _energy = 100; if (_energy < 0) _energy = 0; _saveStats(); }
uint16_t CoreManager::getGlobalColor() { int health = (_energy + _fun + (100 - _hunger)) / 3; if (health > 75) return COLOR_SUCCESS; if (health > 35) return 0xFFE0; return COLOR_DANGER; }
String CoreManager::getTimeString() { struct tm timeinfo; if (!getLocalTime(&timeinfo)) return "--:--"; char timeString[6]; strftime(timeString, sizeof(timeString), "%H:%M", &timeinfo); return String(timeString); }
MoodState CoreManager::getMood() { return _currentMood; }
void CoreManager::setSensorData(float temp, float hum, float lux) { _temp = temp; _hum = hum; _lux = lux; }
float CoreManager::getTemp() { return _temp; }
float CoreManager::getHum()  { return _hum; }
float CoreManager::getLux()  { return _lux; }
int CoreManager::getEnergy() { return (int)_energy; }
int CoreManager::getHunger() { return (int)_hunger; }
int CoreManager::getFun()    { return (int)_fun; }
void CoreManager::setExternalWeather(int code, float temp) { _extWeatherCode = code; _extTemp = temp; }
int CoreManager::getExternalWeatherCode() { return _extWeatherCode; }
float CoreManager::getExternalTemp() { return _extTemp; }