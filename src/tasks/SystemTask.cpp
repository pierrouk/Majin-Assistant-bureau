#include "../Globals.h"
#include "SystemTask.h"

void TaskSystem(void *pvParameters) {
    Serial.println("⚙️ [SysTask] Démarrage Core 0");
    log_i("⚙️ Démarrage Core 0");

    majinUSB.begin();
    majinEnv.begin();  
    majinEyes.begin(); 

    // Démarrage Réseau
    if (majinNet.begin(&majinSettings, &majinCore, &majinVoice, &majinHead)) {
        majinCore.begin(&majinSettings);
        RobotCommand cmd = CMD_WIFI_CONNECTED;
        xQueueSend(guiQueue, &cmd, 0);
        
        // Petit délai pour laisser le stack WiFi respirer après la connexion
        delay(1000); 
        majinNet.fetchWeather();
    } else {
        RobotCommand cmd = CMD_WIFI_FAIL;
        xQueueSend(guiQueue, &cmd, 0);
    }

    unsigned long lastEnvTime = 0;
    unsigned long lastWeatherTime = millis(); 
    
    // Variable pour lisser la luminosité
    float smoothedLux = 50.0; 

    while (true) {
        // 1. Audio & USB
        majinVoice.update();
        majinUSB.update();
        
        // 2. Tactile Tête
        majinTouch.update();
        TouchEventType event = majinTouch.getEvent();
        
        if (event == TOUCH_SINGLE) {
            if (majinCore.getMood() == MOOD_SLEEP) {
                majinCore.wakeUp();
            } else {
                RobotCommand cmd = CMD_HEAD_TAP;
                xQueueSend(guiQueue, &cmd, 0);
            }
        }
        else if (event == TOUCH_LONG) {
            Serial.println("⚙️ [SysTask] Tête: Appui Long détecté !");
            log_i("Tête: Appui Long détecté !");
            RobotCommand cmd = CMD_HEAD_LONG_PRESS;
            xQueueSend(guiQueue, &cmd, 0);
        }

        // 3. Capteurs Locaux (Toutes les 500ms)
        if (millis() - lastEnvTime > 500) {
            lastEnvTime = millis();
            
            EnvData env = majinEnv.read(); 
            float rawLux = majinEyes.getLux();
            
            smoothedLux = (smoothedLux * 0.8) + (rawLux * 0.2);

            float sleepThreshold = majinSettings.getAutoSleepThreshold();
            int brightMin = majinSettings.getScreenMin();
            int brightMax = majinSettings.getScreenMax();
            float daylightLux = 400.0; 

            if (env.valid) {
                majinCore.setSensorData(env.temperature, env.humidity, smoothedLux);
            }
            
            // Auto Brightness
            int targetBright = map((long)smoothedLux, 0, (long)daylightLux, brightMin, brightMax);
            if (targetBright > 255) targetBright = 255;
            if (targetBright < brightMin) targetBright = brightMin;

            majinScreen.setBrightness(targetBright);
            majinCore.update();
        }
        
        // 4. MÉTÉO INTELLIGENTE (RETRY LOGIC)
        // Si on a pas encore la météo (code -1), on réessaye vite (5s)
        // Si on l'a déjà, on attend le cycle normal (10min = 600000ms)
        unsigned long weatherInterval = (majinCore.getExternalWeatherCode() == -1) ? 5000 : 600000;

        if (millis() - lastWeatherTime > weatherInterval) {
            lastWeatherTime = millis();
            
            // On ne tente que si le WiFi est connecté
            if (majinNet.isConnected()) {
                log_i("🌦️ [System]: Tentative mise à jour météo...");
                log_i("🌦️ Tentative mise à jour météo...");
                majinNet.fetchWeather();
            }
        }

        vTaskDelay(pdMS_TO_TICKS(10));
    }
}