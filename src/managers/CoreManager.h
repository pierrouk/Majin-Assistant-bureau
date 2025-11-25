#ifndef CORE_MANAGER_H
#define CORE_MANAGER_H

#include <Arduino.h>
#include <WiFi.h>
#include <time.h>
#include "../../include/config.h"
#include "SettingsManager.h" 

enum MoodState {
    MOOD_NEUTRAL,
    MOOD_HAPPY,
    MOOD_ANGRY,
    MOOD_SLEEP,
    MOOD_TIRED,
    MOOD_HUNGRY,
    MOOD_BORED
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
    float getTemp();
    float getHum();
    float getLux();

    // --- Actions Tamagotchi ---
    void feed(int amount); 
    void play(int amount); 
    void sleep(bool force); 
    void wakeUp();
    
    // 🛠️ CORRECTIF : Nouvelle méthode pour le reset total
    void resetLife();

    // --- Getters Jauges ---
    int getEnergy();
    int getHunger();
    int getFun();
    
    // Ajout helper pour changer l'énergie manuellement si besoin
    void changeEnergy(int delta);

    uint16_t getGlobalColor(); 

private:
    SettingsManager* _settings;
    MoodState _currentMood = MOOD_NEUTRAL;
    
    float _temp = 0.0f; float _hum = 0.0f; float _lux = 0.0f;

    float _energy = 100; 
    float _hunger = 0;   
    float _fun = 100;    

    bool _isSleeping = false;

    void _calculateMood();
    void _decayStats(); 
    void _saveStats();  
};

#endif