#ifndef NETWORK_MANAGER_H
#define NETWORK_MANAGER_H

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiManager.h>
#include <ESPAsyncWebServer.h>
#include <HTTPClient.h> // ⬅️ NOUVEAU : Pour appeler l'API
#include <ArduinoJson.h> // ⬅️ NOUVEAU : Pour parser
#include <functional> 
#include "SettingsManager.h" 
#include "WebPages.h" 
#include "CoreManager.h"
#include "../hal/SoundSystem.h"
#include "../hal/ServoMotor.h" 

class NetworkManager {
public:
    NetworkManager();
    bool begin(SettingsManager* settings, CoreManager* core, SoundSystem* sound, ServoMotor* servo);
    void setupWebServer();
    String getIP();
    bool isConnected();
    void resetWiFi();

    // ⬅️ NOUVEAU : Récupération météo (appelé par TaskSystem)
    void fetchWeather(); 

    using VoidCallback = std::function<void()>;
    using IntCallback  = std::function<void(int)>;
    void onTriggerHello(VoidCallback cb);
    void onTriggerDance(VoidCallback cb);
    void onTriggerMood(IntCallback cb);

    // ⬅️ NOUVEAU : Callback pour mettre à jour l'effet UI
    using WeatherCallback = std::function<void(int code)>;
    void onWeatherUpdate(WeatherCallback cb);

private:
    AsyncWebServer _server;
    bool _connected = false;
    SettingsManager* _settings; 
    CoreManager* _core;         
    SoundSystem* _sound;
    ServoMotor* _servo;

    VoidCallback _helloCallback = nullptr;
    VoidCallback _danceCallback = nullptr;
    IntCallback  _moodCallback  = nullptr;
    WeatherCallback _weatherCallback = nullptr;
};
#endif