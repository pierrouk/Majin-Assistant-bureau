#include "../Globals.h"
#include "SystemTask.h"

// Note: Les #define sont supprimés car on utilise SettingsManager maintenant

void TaskSystem(void *pvParameters) {
    Serial.println("⚙️ [SysTask] Démarrage Core 0");

    majinUSB.begin();
    majinEnv.begin();  
    majinEyes.begin(); 

    if (majinNet.begin(&majinSettings, &majinCore, &majinVoice, &majinHead)) {
        majinCore.begin(&majinSettings);
        RobotCommand cmd = CMD_WIFI_CONNECTED;
        xQueueSend(guiQueue, &cmd, 0);
        majinNet.fetchWeather();
    } else {
        RobotCommand cmd = CMD_WIFI_FAIL;
        xQueueSend(guiQueue, &cmd, 0);
    }

    unsigned long lastEnvTime = 0;
    unsigned long lastWeatherTime = millis(); 
    
    float smoothedLux = 50.0; 

    while (true) {
        majinVoice.update();
        majinUSB.update();
        
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
            RobotCommand cmd = CMD_HEAD_LONG_PRESS;
            xQueueSend(guiQueue, &cmd, 0);
        }

        // 4. Capteurs Locaux (Toutes les 500ms)
        if (millis() - lastEnvTime > 500) {
            lastEnvTime = millis();
            
            // Lecture des capteurs ICI
            EnvData env = majinEnv.read(); // Déclaration locale
            float rawLux = majinEyes.getLux();
            
            // Lissage
            smoothedLux = (smoothedLux * 0.8) + (rawLux * 0.2);

            // Settings Dynamiques
            float sleepThreshold = majinSettings.getAutoSleepThreshold();
            int brightMin = majinSettings.getScreenMin();
            int brightMax = majinSettings.getScreenMax();
            float daylightLux = 400.0; 

            // Envoi au Core
            if (env.valid) {
                majinCore.setSensorData(env.temperature, env.humidity, smoothedLux);
            }
            
            // Auto Brightness
            int targetBright = map((long)smoothedLux, 20, (long)daylightLux, brightMin, brightMax);
            if (targetBright > 255) targetBright = 255;
            if (targetBright < brightMin) targetBright = brightMin;

            majinScreen.setBrightness(targetBright);
            majinCore.update();
        }
        
        if (millis() - lastWeatherTime > 600000) {
            lastWeatherTime = millis();
            majinNet.fetchWeather();
        }

        vTaskDelay(pdMS_TO_TICKS(10));
    }
}