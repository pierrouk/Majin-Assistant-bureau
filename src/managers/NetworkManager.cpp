#include "NetworkManager.h"

NetworkManager::NetworkManager() : _server(80) {
}

bool NetworkManager::begin(SettingsManager* settings, CoreManager* core, SoundSystem* sound, ServoMotor* servo) {
    _settings = settings;
    _core = core;
    _sound = sound;
    _servo = servo;

    Serial.println("📡 [Network]: Démarrage WiFiManager...");

    WiFiManager wifiManager;
    String customHead = WebTheme::getCommonCSS() + WebTheme::getSetupInjectionJS();
    wifiManager.setCustomHeadElement(customHead.c_str());
    
    std::vector<const char *> menu = {"wifi", "update"};
    wifiManager.setMenu(menu);
    wifiManager.setTitle("Majin OS Setup");
    wifiManager.setDebugOutput(false); 
    wifiManager.setConfigPortalTimeout(180);

    String currentName = settings->getRobotName();
    WiFiManagerParameter custom_robot_name("robot_name", "Donne-moi un nom (Requis)", currentName.c_str(), 32);
    wifiManager.addParameter(&custom_robot_name);

    bool res = wifiManager.autoConnect("Majin_Setup", "majin1234");

    if(!res) {
        Serial.println("🔴 [Network]: Échec connexion.");
        _connected = false;
        return false;
    } 

    String newName = custom_robot_name.getValue();
    newName.trim();
    if (newName.length() > 0 && newName != currentName) {
        settings->setRobotName(newName);
    }

    Serial.println("🟢 [Network]: Connecté !");
    _connected = true;
    setupWebServer();
    return true;
}

void NetworkManager::resetWiFi() {
    WiFiManager wifiManager;
    wifiManager.resetSettings();
    Serial.println("📡 [Network]: Creds WiFi effacées.");
}

void NetworkManager::onTriggerHello(VoidCallback cb) { _helloCallback = cb; }
void NetworkManager::onTriggerDance(VoidCallback cb) { _danceCallback = cb; }
void NetworkManager::onTriggerMood(IntCallback cb)  { _moodCallback = cb; }
void NetworkManager::onWeatherUpdate(WeatherCallback cb) { _weatherCallback = cb; }

void NetworkManager::fetchWeather() {
    // Si désactivé, on ne fetch pas. L'UI le cachera grâce au settings->getWeatherEnabled()
    if (!_settings->getWeatherEnabled()) {
        Serial.println("🚫 [Network]: Météo désactivée");
        return;
    }

    if (!_connected) return;

    String lat = _settings->getWeatherLat();
    String lon = _settings->getWeatherLon();

    Serial.println("🌦️ [Network]: Mise à jour Météo...");

    HTTPClient http;
    String url = "http://api.open-meteo.com/v1/forecast?latitude=" + lat + "&longitude=" + lon + "&current_weather=true";
    
    http.begin(url);
    int httpCode = http.GET();

    if (httpCode == 200) {
        String payload = http.getString();
        DynamicJsonDocument doc(1024);
        deserializeJson(doc, payload);
        
        int weatherCode = doc["current_weather"]["weathercode"];
        float temp = doc["current_weather"]["temperature"];
        
        Serial.printf("🌦️ [Network]: Météo Code WMO: %d, Temp: %.1f\n", weatherCode, temp);

        _core->setExternalWeather(weatherCode, temp);

        if (_weatherCallback) _weatherCallback(weatherCode);
        
    } else {
        Serial.printf("🔴 [Network]: Erreur HTTP Météo: %d\n", httpCode);
    }
    http.end();
}

void NetworkManager::setupWebServer() {
    _server.on("/", HTTP_GET, [this](AsyncWebServerRequest *request){ request->send(200, "text/html", PageDashboard::getHTML()); });

    _server.on("/modules", HTTP_GET, [this](AsyncWebServerRequest *request){
        bool buz = _settings->getBuzzerEnabled();
        bool srv = _settings->getServoEnabled();
        request->send(200, "text/html", PageModules::getHTML(buz, srv));
    });

    _server.on("/settings", HTTP_GET, [this](AsyncWebServerRequest *request){ request->send(200, "text/html", PageSettings::getHTML()); });

    // 🛠️ MISE À JOUR : Route Perso avec City
    _server.on("/perso", HTTP_GET, [this](AsyncWebServerRequest *request){
        bool t = _settings->getShowTime();
        bool s = _settings->getShowSensors();
        String lat = _settings->getWeatherLat();
        String lon = _settings->getWeatherLon();
        bool en = _settings->getWeatherEnabled();
        String city = _settings->getCityName(); // Récup Ville
        String boot = _settings->getBootLogoText();
        int mood = _settings->getManualMood();
        // Passage de 'city' à la page
        request->send(200, "text/html", PagePersonalization::getHTML(t, s, lat, lon, en, city, boot, mood));
    });

    _server.on("/countdown", HTTP_GET, [this](AsyncWebServerRequest *request){
        request->send(200, "text/html", PageCountdown::getHTML(_settings->getEventName(), _settings->getEventTimestamp(), _settings->getEventType()));
    });

    _server.on("/tamagotchi", HTTP_GET, [this](AsyncWebServerRequest *request){ request->send(200, "text/html", PageTamagotchi::getHTML()); });

    _server.on("/deck", HTTP_GET, [this](AsyncWebServerRequest *request){
        int page = 0; if(request->hasParam("page")) page = request->getParam("page")->value().toInt();
        request->send(200, "text/html", PageDeckConfig::getHTML(_settings, page));
    });

    _server.on("/api/status", HTTP_GET, [this](AsyncWebServerRequest *request){
        String json = "{";
        json += "\"name\":\"" + _settings->getRobotName() + "\",";
        json += "\"ip\":\"" + getIP() + "\",";
        json += "\"rssi\":" + String(WiFi.RSSI()) + ",";
        float t = (_core) ? _core->getTemp() : 0.0;
        float h = (_core) ? _core->getHum() : 0.0;
        float l = (_core) ? _core->getLux() : 0.0;
        int e   = (_core) ? _core->getEnergy() : 0;
        json += "\"temp\":" + String(t, 1) + ",";
        json += "\"hum\":" + String(h, 0) + ",";
        json += "\"lux\":" + String(l, 0) + ",";
        json += "\"energy\":" + String(e) + "";
        json += "}";
        request->send(200, "application/json", json);
    });

    _server.on("/api/tamagotchi/status", HTTP_GET, [this](AsyncWebServerRequest *request){
        String json = "{";
        json += "\"energy\":" + String(_core->getEnergy()) + ",";
        json += "\"hunger\":" + String(_core->getHunger()) + ",";
        json += "\"fun\":" + String(_core->getFun());
        json += "}";
        request->send(200, "application/json", json);
    });

    _server.on("/api/hello", HTTP_GET, [this](AsyncWebServerRequest *request){ if (_helloCallback) _helloCallback(); request->send(200, "text/plain", "OK"); });
    _server.on("/api/dance", HTTP_GET, [this](AsyncWebServerRequest *request){ if (_danceCallback) _danceCallback(); request->send(200, "text/plain", "OK"); });
    _server.on("/api/mood", HTTP_GET, [this](AsyncWebServerRequest *request){ if (request->hasParam("val")) { int val = request->getParam("val")->value().toInt(); if (_moodCallback) _moodCallback(val); } request->send(200, "text/plain", "OK"); });

    _server.on("/api/module/set", HTTP_POST, [this](AsyncWebServerRequest *request){
        if (request->hasParam("id") && request->hasParam("state")) {
            String id = request->getParam("id")->value();
            bool state = (request->getParam("state")->value() == "1");
            if (id == "buzzer") { _settings->setBuzzerEnabled(state); _sound->setEnabled(state); } 
            else if (id == "servo") { _settings->setServoEnabled(state); _servo->setEnabled(state); }
            request->send(200, "text/plain", "OK");
        } else { request->send(400); }
    });

    _server.on("/api/module/test", HTTP_POST, [this](AsyncWebServerRequest *request){
        if (request->hasParam("id")) {
            String id = request->getParam("id")->value();
            if (id == "buzzer") { _sound->testSequence(); } 
            else if (id == "servo") { _servo->testSequence(); }
            request->send(200, "text/plain", "Test OK");
        } else { request->send(400); }
    });

    // 🛠️ MISE À JOUR : Gestion Paramètre Ville + Enable
    _server.on("/api/config/weather", HTTP_POST, [this](AsyncWebServerRequest *request){
        if (request->hasParam("lat")) {
            _settings->setWeatherLat(request->getParam("lat")->value());
            _settings->setWeatherLon(request->getParam("lon")->value());
            
            if (request->hasParam("city")) {
                _settings->setCityName(request->getParam("city")->value());
            }

            bool enable = (request->getParam("enable")->value() == "1");
            _settings->setWeatherEnabled(enable);

            if (enable) {
                fetchWeather(); // Mise à jour immédiate si activé
            }
            
            request->send(200, "text/plain", "OK");
        } else { request->send(400); }
    });

    _server.on("/api/config/display", HTTP_POST, [this](AsyncWebServerRequest *request){
        if (request->hasParam("id") && request->hasParam("state")) {
            String id = request->getParam("id")->value();
            bool state = (request->getParam("state")->value() == "1");
            if (id == "time") _settings->setShowTime(state);
            else if (id == "sensors") _settings->setShowSensors(state);
            request->send(200, "text/plain", "OK");
        } else { request->send(400); }
    });

    _server.on("/api/config/mood", HTTP_POST, [this](AsyncWebServerRequest *request){
        if (request->hasParam("val")) {
            int val = request->getParam("val")->value().toInt();
            _settings->setManualMood(val);
            if (val == -1) _core->wakeUp(); 
            request->send(200, "text/plain", "OK");
        } else { request->send(400); }
    });

    _server.on("/api/countdown/set", HTTP_POST, [this](AsyncWebServerRequest *request){
        if (request->hasParam("name", true) && request->hasParam("timestamp", true)) {
            String name = request->getParam("name", true)->value();
            String tsStr = request->getParam("timestamp", true)->value();
            String typeStr = request->hasParam("type", true) ? request->getParam("type", true)->value() : "0";
            unsigned long ts = strtoul(tsStr.c_str(), NULL, 10);
            int type = typeStr.toInt();
            _settings->setEventName(name); _settings->setEventTimestamp(ts); _settings->setEventType(type);
            request->redirect("/countdown");
        } else { request->send(400); }
    });

    _server.on("/api/countdown/delete", HTTP_POST, [this](AsyncWebServerRequest *request){
        _settings->setEventName(""); _settings->setEventTimestamp(0); request->send(200, "text/plain", "Deleted");
    });

    _server.on("/api/tamagotchi/action", HTTP_POST, [this](AsyncWebServerRequest *request){
        if (request->hasParam("type")) {
            String type = request->getParam("type")->value();
            if (type == "feed") _core->feed(20);      
            if (type == "play") _core->play(15);      
            if (type == "sleep") _core->sleep(true);  
            if (type == "wake") _core->wakeUp();      
            if (type == "reset_stats") _core->resetLife();
            request->send(200, "text/plain", "OK");
        } else { request->send(400); }
    });

    _server.on("/api/deck/set", HTTP_POST, [this](AsyncWebServerRequest *request){
        if (request->hasParam("id")) {
            int id = request->getParam("id")->value().toInt();
            if (request->hasParam("active") && request->getParam("active")->value() == "0") {
                DeckButton empty = {"", 0, 0, false, false, false, false, 0};
                _settings->setDeckButton(id, empty);
                request->send(200, "text/plain", "Cleared");
                return;
            }
            DeckButton btn;
            String label = request->getParam("label")->value();
            if(label.length() > 9) label = label.substring(0, 9);
            label.toCharArray(btn.label, 10);
            String keyStr = request->getParam("key")->value();
            if(keyStr.length() > 0) btn.key = keyStr.charAt(0);
            btn.color = request->getParam("color")->value().toInt();
            btn.iconID = request->hasParam("icon") ? request->getParam("icon")->value().toInt() : 0;
            btn.ctrl = (request->getParam("ctrl")->value() == "1");
            btn.shift = (request->getParam("shift")->value() == "1");
            btn.alt = (request->getParam("alt")->value() == "1");
            btn.active = true;
            _settings->setDeckButton(id, btn);
            request->send(200, "text/plain", "Saved");
        } else { request->send(400); }
    });

    _server.on("/api/wifi/reset", HTTP_POST, [this](AsyncWebServerRequest *request){ request->send(200); delay(500); resetWiFi(); ESP.restart(); });
    _server.on("/api/reset", HTTP_POST, [this](AsyncWebServerRequest *request){ request->send(200); delay(500); resetWiFi(); _settings->factoryReset(); ESP.restart(); });

    _server.begin();
    Serial.println("🌐 [Network]: Serveur Web démarré");
}

String NetworkManager::getIP() { return _connected ? WiFi.localIP().toString() : "0.0.0.0"; }
bool NetworkManager::isConnected() { return (WiFi.status() == WL_CONNECTED); }