#include "Globals.h"
#include "tasks/SystemTask.h"
#include "tasks/GuiTask.h"

// --- Instanciation des Objets Globaux ---
LightSensor       majinEyes;
ServoMotor        majinHead;
TouchSensor       majinTouch;
SoundSystem       majinVoice;
DisplayDriver     majinScreen;
EnvironmentSensor majinEnv; 
HapticDriver majinHaptic(VIBE_PIN); // ⬅️ Instanciation sur la Pin 41

NetworkManager    majinNet;
CoreManager       majinCore;
UIManager         majinUI;
InputManager      majinInput; 
UsbManager        majinUSB; 
SettingsManager   majinSettings; 
BootSequence      majinBoot;     

QueueHandle_t guiQueue;

void setup() {
    Serial.begin(115200);
    delay(1000); 
    Serial.println("\n=== MAJIN OS v3.1 : MODULAR TASK SYSTEM ===");

    Wire.begin(ESP32_I2C_SDA_PIN, ESP32_I2C_SCL_PIN);

    // 1. Init Drivers Hardware
    majinVoice.begin();
    majinHead.begin();
    majinTouch.begin();
    majinEyes.begin(&Wire);
    majinEnv.begin();
    majinHaptic.begin(); // ⬅️ Démarrage du vibreur
    
    majinHaptic.doubleBuzz(); // "Bzz Bzz" au démarrage pour confirmer que ça marche
    // 2. Init Settings
    majinSettings.begin();

    // 3. Sync Hardware avec Settings
    majinHead.setEnabled(majinSettings.getServoEnabled());
    majinVoice.setEnabled(majinSettings.getBuzzerEnabled());

    // 4. Séquence de Boot (Bloquant)
    if(majinScreen.begin()) {
        majinBoot.run(&majinHead, &majinVoice, &majinScreen, 
                      &majinSettings, &majinUI, &majinCore, &majinNet); 
    }

    // 5. Création Queue & Callbacks
    guiQueue = xQueueCreate(10, sizeof(RobotCommand));

    majinNet.onTriggerHello([]() { RobotCommand c=CMD_HELLO; xQueueSend(guiQueue, &c, 0); });
    majinNet.onTriggerDance([]() { RobotCommand c=CMD_DANCE; xQueueSend(guiQueue, &c, 0); });
    majinNet.onTriggerMood([](int val) {
        majinCore.setMood((val == 1) ? MOOD_HAPPY : MOOD_ANGRY);
        RobotCommand c=(val==1)?CMD_MOOD_HAPPY:CMD_MOOD_ANGRY;
        xQueueSend(guiQueue, &c, 0);
    });

    // Callback Météo
    majinNet.onWeatherUpdate([](int code) {
        UIEffect effect = EFFECT_NONE;
        if (code == 0) effect = EFFECT_SUN;
        else if (code >= 1 && code <= 48) effect = EFFECT_CLOUDS;
        else if ((code >= 51 && code <= 67) || (code >= 80 && code <= 82) || code >= 95) effect = EFFECT_RAIN;
        else if (code >= 71 && code <= 86) effect = EFFECT_SNOW;
        majinUI.setWeatherEffect(effect);
    });

    // 6. Lancement des Tâches
    Serial.println("🚀 Kernel Start");
    xTaskCreatePinnedToCore(TaskSystem, "SystemTask", 4096, NULL, 2, NULL, 0);
    xTaskCreatePinnedToCore(TaskGUI,    "GuiTask",    8192, NULL, 1, NULL, 1);
}

void loop() { vTaskDelete(NULL); }