#include "../Globals.h"
#include "SystemTask.h"

void TaskSystem(void *pvParameters) {
    Serial.println("⚙️ [SysTask] Démarrage Core 0");

    // Démarrage USB HID
    majinUSB.begin();

    // Démarrage Réseau & Services
    // On passe les pointeurs hardware pour le contrôle via Web
    if (majinNet.begin(&majinSettings, &majinCore, &majinVoice, &majinHead)) {
        majinCore.begin(&majinSettings);
        
        RobotCommand cmd = CMD_WIFI_CONNECTED;
        xQueueSend(guiQueue, &cmd, 0);
        
        // Force météo immédiate
        majinNet.fetchWeather();
    } else {
        RobotCommand cmd = CMD_WIFI_FAIL;
        xQueueSend(guiQueue, &cmd, 0);
    }

    unsigned long lastEnvTime = 0;
    unsigned long lastWeatherTime = millis(); 

    while (true) {
        // 1. Moteur Audio (Prioritaire)
        majinVoice.update();
        
        // 2. Moteur Jiggler (USB)
        majinUSB.update();
        
        // 3. Gestion Tactile Tête (Hardware)
        majinTouch.update();
        TouchEventType event = majinTouch.getEvent();
        
        if (event == TOUCH_SINGLE) {
            // Tap simple -> Interaction
            RobotCommand cmd = CMD_HEAD_TAP;
            xQueueSend(guiQueue, &cmd, 0);
        }
        else if (event == TOUCH_LONG) {
            // Appui Long -> Sortie de secours
            Serial.println("⚙️ [SysTask] Tête: Appui Long détecté !");
            RobotCommand cmd = CMD_HEAD_LONG_PRESS;
            xQueueSend(guiQueue, &cmd, 0);
        }

        // 4. Capteurs Locaux (Toutes les 2s)
        if (millis() - lastEnvTime > 2000) {
            lastEnvTime = millis();
            EnvData env = majinEnv.read();
            float lux = majinEyes.getLux();
            if (env.valid) {
                majinCore.setSensorData(env.temperature, env.humidity, lux);
            }
            // Mise à jour de la vie (Tamagotchi)
            majinCore.update();
        }
        
        // 5. Météo Web (Toutes les 10 min)
        if (millis() - lastWeatherTime > 600000) {
            lastWeatherTime = millis();
            majinNet.fetchWeather();
        }

        // Pause courte pour laisser respirer le Watchdog
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}