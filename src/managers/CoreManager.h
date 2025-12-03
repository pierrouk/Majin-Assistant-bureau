#ifndef CORE_MANAGER_H
#define CORE_MANAGER_H

#include <Arduino.h>
#include <WiFi.h>
#include <time.h>
#include "../../include/config.h"
#include "SettingsManager.h" 

enum MoodState {
    MOOD_NEUTRAL, MOOD_HAPPY, MOOD_ANGRY, MOOD_SAD, 
    MOOD_SLEEP, MOOD_LOVE, MOOD_TIRED, MOOD_HUNGRY, MOOD_BORED
};

class CoreManager {
public:
    CoreManager();
    void begin(SettingsManager* settings);
    void update(); 

    String getTimeString();
    
    void setMood(MoodState newMood);
    MoodState getMood();
    
    // --- Capteurs ---
    void setSensorData(float temp, float hum, float lux);
    float getTemp(); float getHum(); float getLux();

    // --- Météo Externe ---
    void setExternalWeather(int code, float temp);
    int getExternalWeatherCode(); float getExternalTemp();

    // --- Actions ---
    void feed(int amount); void play(int amount); 
    void sleep(bool force); void wakeUp(); void resetLife();

    // --- Stats ---
    int getEnergy(); int getHunger(); int getFun();
    void changeEnergy(int delta);
    uint16_t getGlobalColor(); 

private:
    SettingsManager* _settings;
    MoodState _currentMood = MOOD_NEUTRAL;
    
    float _temp = 0.0f; float _hum = 0.0f; float _lux = 0.0f;
    int _extWeatherCode = -1; float _extTemp = 0.0f;

    float _energy = 100; float _hunger = 0; float _fun = 100;    
    bool _isSleeping = false;

    // 💡 TIMER AUTO-SLEEP
    unsigned long _darknessTimer = 0; 

    void _calculateMood();
    void _decayStats(); 
    void _saveStats();  
    void _checkAutoSleep(); // Nouvelle fonction privée
};

#endif