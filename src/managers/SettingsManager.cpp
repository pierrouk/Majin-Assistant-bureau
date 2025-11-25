#include "SettingsManager.h"
#include "../../include/Theme.h" 

SettingsManager::SettingsManager() {}

void SettingsManager::begin() {
    _prefs.begin(_namespace, false);
    _loadCache(); 
    Serial.println("💾 [Settings]: Mémoire chargée.");
}

void SettingsManager::_loadCache() {
    _cache_showTime = _prefs.getBool("show_time", true);
    _cache_showSensors = _prefs.getBool("show_sensors", true);
    _cache_servoEnabled = _prefs.getBool("srv_en", true);
    _cache_buzzerEnabled = _prefs.getBool("buz_en", true);
    _cache_volume = _prefs.getUChar("volume", 50);
    _cache_brightness = _prefs.getUChar("brightness", 200);
    _cache_name = _prefs.getString("name", "Majin Bot");
    _cache_weatherEnabled = _prefs.getBool("w_en", false);
    _cache_lat = _prefs.getString("w_lat", "48.8566");
    _cache_lon = _prefs.getString("w_lon", "2.3522");
    _cache_bootText = _prefs.getString("boot_txt", "MAJIN OS");
    
    // ⬅️ Chargement Mood (-1 par défaut si non existant)
    _cache_manualMood = _prefs.getInt("man_mood", -1);

    if (_prefs.isKey("deck_data") && _prefs.getBytesLength("deck_data") == sizeof(_cache_deck)) {
        _prefs.getBytes("deck_data", _cache_deck, sizeof(_cache_deck));
    } else {
        _cache_deck[0] = (DeckButton){"MUTE", COLOR_PRIMARY, 'm', false, false, false, true, 2};
        _cache_deck[1] = (DeckButton){"COPY", COLOR_SUCCESS, 'c', true, false, false, true, 6};
        _cache_deck[2] = (DeckButton){"PASTE", COLOR_ACCENT, 'v', true, false, false, true, 7};
        for(int i=3; i<DECK_TOTAL_BTNS; i++) _cache_deck[i] = (DeckButton){"", 0, 0, false, false, false, false, 0};
        _prefs.putBytes("deck_data", _cache_deck, sizeof(_cache_deck));
    }
}

bool SettingsManager::isSetupDone() { return _prefs.getBool("setup_done", false); }
void SettingsManager::setSetupDone(bool done) { _prefs.putBool("setup_done", done); }
void SettingsManager::setVolume(uint8_t volume) { if (volume > 100) volume = 100; _cache_volume = volume; _prefs.putUChar("volume", volume); }
uint8_t SettingsManager::getVolume() { return _cache_volume; }
void SettingsManager::setScreenBrightness(uint8_t brightness) { _cache_brightness = brightness; _prefs.putUChar("brightness", brightness); }
uint8_t SettingsManager::getScreenBrightness() { return _cache_brightness; }
void SettingsManager::setRobotName(String name) { _cache_name = name; _prefs.putString("name", name); }
String SettingsManager::getRobotName() { return _cache_name; }

// --- PERSONNALISATION ---
void SettingsManager::setShowTime(bool show) { _cache_showTime = show; _prefs.putBool("show_time", show); }
bool SettingsManager::getShowTime() { return _cache_showTime; }
void SettingsManager::setShowSensors(bool show) { _cache_showSensors = show; _prefs.putBool("show_sensors", show); }
bool SettingsManager::getShowSensors() { return _cache_showSensors; }

// ⬅️ MANUAL MOOD
void SettingsManager::setManualMood(int moodId) { 
    _cache_manualMood = moodId; 
    _prefs.putInt("man_mood", moodId); 
}
int SettingsManager::getManualMood() { 
    return _cache_manualMood; 
}

void SettingsManager::setWeatherLat(String lat) { _cache_lat = lat; _prefs.putString("w_lat", lat); }
String SettingsManager::getWeatherLat() { return _cache_lat; }
void SettingsManager::setWeatherLon(String lon) { _cache_lon = lon; _prefs.putString("w_lon", lon); }
String SettingsManager::getWeatherLon() { return _cache_lon; }
void SettingsManager::setWeatherEnabled(bool enabled) { _cache_weatherEnabled = enabled; _prefs.putBool("w_en", enabled); }
bool SettingsManager::getWeatherEnabled() { return _cache_weatherEnabled; }
void SettingsManager::setServoEnabled(bool enabled) { _cache_servoEnabled = enabled; _prefs.putBool("srv_en", enabled); }
bool SettingsManager::getServoEnabled() { return _cache_servoEnabled; }
void SettingsManager::setBuzzerEnabled(bool enabled) { _cache_buzzerEnabled = enabled; _prefs.putBool("buz_en", enabled); }
bool SettingsManager::getBuzzerEnabled() { return _cache_buzzerEnabled; }
void SettingsManager::setBootLogoText(String text) { if (text.length() > 15) text = text.substring(0, 15); _cache_bootText = text; _prefs.putString("boot_txt", text); }
String SettingsManager::getBootLogoText() { return _cache_bootText; }
void SettingsManager::setEventName(String name) { _prefs.putString("evt_name", name); }
String SettingsManager::getEventName() { return _prefs.getString("evt_name", ""); }
void SettingsManager::setEventTimestamp(unsigned long timestamp) { _prefs.putULong("evt_time", timestamp); }
unsigned long SettingsManager::getEventTimestamp() { return _prefs.getULong("evt_time", 0); }
void SettingsManager::setEventType(int type) { _prefs.putInt("evt_type", type); }
int SettingsManager::getEventType() { return _prefs.getInt("evt_type", 0); }
void SettingsManager::setStats(int energy, int hunger, int fun) { _prefs.putInt("t_energy", energy); _prefs.putInt("t_hunger", hunger); _prefs.putInt("t_fun", fun); }
int SettingsManager::getStatEnergy() { return _prefs.getInt("t_energy", 100); } 
int SettingsManager::getStatHunger() { return _prefs.getInt("t_hunger", 0); }   
int SettingsManager::getStatFun()    { return _prefs.getInt("t_fun", 100); }    

void SettingsManager::setDeckButton(int index, DeckButton btn) {
    if (index < 0 || index >= DECK_TOTAL_BTNS) return;
    _cache_deck[index] = btn;
    _prefs.putBytes("deck_data", _cache_deck, sizeof(_cache_deck));
}

DeckButton SettingsManager::getDeckButton(int index) {
    if (index < 0 || index >= DECK_TOTAL_BTNS) return (DeckButton){"", 0, 0, false, false, false, false, 0};
    return _cache_deck[index];
}

void SettingsManager::factoryReset() {
    Serial.println("⚠️ [Settings]: FACTORY RESET...");
    _prefs.clear(); 
    _loadCache();
    Serial.println("💾 [Settings]: Mémoire vidée.");
}