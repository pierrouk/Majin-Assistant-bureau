#ifndef SETTINGS_MANAGER_H
#define SETTINGS_MANAGER_H

#include <Arduino.h>
#include <Preferences.h> 

#define DECK_PAGES 4
#define DECK_BTNS_PER_PAGE 6
#define DECK_TOTAL_BTNS (DECK_PAGES * DECK_BTNS_PER_PAGE)

struct DeckButton {
    char label[10];   
    uint32_t color;   
    uint8_t key;      
    bool ctrl;
    bool shift;
    bool alt;
    bool active;      
    uint8_t iconID; 
};

class SettingsManager {
public:
    SettingsManager();
    void begin();

    bool isSetupDone(); void setSetupDone(bool done);
    void setVolume(uint8_t volume); uint8_t getVolume();
    void setScreenBrightness(uint8_t brightness); uint8_t getScreenBrightness();
    void setRobotName(String name); String getRobotName();

    // --- Personnalisation ---
    void setShowTime(bool show); bool getShowTime();
    void setShowSensors(bool show); bool getShowSensors();
    
    // ⬅️ NOUVEAU : Forçage de l'expression (-1 = Auto)
    void setManualMood(int moodId); 
    int getManualMood();

    // --- Météo ---
    void setWeatherLat(String lat); String getWeatherLat();
    void setWeatherLon(String lon); String getWeatherLon();
    void setWeatherEnabled(bool enabled); bool getWeatherEnabled();

    // --- Hardware ---
    void setServoEnabled(bool enabled); bool getServoEnabled();
    void setBuzzerEnabled(bool enabled); bool getBuzzerEnabled();
    
    void setBootLogoText(String text); String getBootLogoText();

    void setEventName(String name); String getEventName();
    void setEventTimestamp(unsigned long timestamp); unsigned long getEventTimestamp();
    void setEventType(int type); int getEventType();
    void setStats(int energy, int hunger, int fun);
    int getStatEnergy(); int getStatHunger(); int getStatFun();    

    void setDeckButton(int index, DeckButton btn);
    DeckButton getDeckButton(int index); 

    void factoryReset(); 

private:
    Preferences _prefs;
    const char* _namespace = "majin_cfg"; 

    bool _cache_showTime = true;
    bool _cache_showSensors = true;
    bool _cache_servoEnabled = true;
    bool _cache_buzzerEnabled = true;
    bool _cache_weatherEnabled = false;
    uint8_t _cache_volume = 50;
    uint8_t _cache_brightness = 200;
    String _cache_name = "Majin Bot";
    String _cache_lat = "48.8566"; 
    String _cache_lon = "2.3522";
    String _cache_bootText = "MAJIN OS";
    
    // ⬅️ Cache Mood
    int _cache_manualMood = -1; // -1 = Auto

    DeckButton _cache_deck[DECK_TOTAL_BTNS];

    void _loadCache();
};

#endif