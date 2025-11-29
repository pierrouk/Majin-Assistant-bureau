#include "../Globals.h"
#include "GuiTask.h"

void TaskGUI(void *pvParameters) {
    Serial.println("🎨 [GuiTask] Démarrage Core 1");

    if (!majinScreen.begin()) { 
        Serial.println("❌ GUI: Erreur Ecran"); 
    } else { 
        // Init du Manager UI sur le bon coeur
        majinUI.begin(&majinScreen, &majinCore, &majinNet, &majinSettings, &majinUSB); 
        majinVoice.playSuccess(); 
    }
    
    // Nettoyage écran post-boot
    majinScreen.secureDraw([](LGFX_Majin* tft) { tft->fillScreen(0x0000); }); // Noir

    RobotCommand currentCmd;
    uint16_t touchX = 0, touchY = 0;
    bool isTouched = false;
    unsigned long lastIdleMove = 0;
    unsigned long nextIdleMoveDelay = 3000; 

    while (true) {
        // 1. ACQUISITION
        isTouched = majinScreen.getTouchSafe(&touchX, &touchY);
        // ⬅️ FEEDBACK HAPTIQUE
        // Si on vient de toucher l'écran (Front montant), on fait un petit clic
        static bool wasTouched = false;
        if (isTouched && !wasTouched) {
            majinHaptic.click(); 
        }
        wasTouched = isTouched;
        GestureType gesture = majinInput.process(isTouched, touchX, touchY);
        UIScene currentScene = majinUI.getScene();

        // 2. COMPORTEMENT IDLE (Vivant)
        if (currentScene == SCENE_FACE) {
             if (millis() - lastIdleMove > nextIdleMoveDelay) {
                lastIdleMove = millis();
                nextIdleMoveDelay = random(2000, 8000); 
                // Petit mouvement de tête aléatoire
                majinHead.setAngle(random(70, 110));
            }
        }

        // 3. GESTION DES GESTES (Navigation)
        if (gesture != GESTURE_NONE) {
            
            // A. SCÈNE VISAGE (Accueil)
            if (currentScene == SCENE_FACE) {
                if (!majinSettings.isSetupDone()) {
                   // Mode "Premier Démarrage"
                   if (gesture == GESTURE_TAP || gesture == GESTURE_SWIPE_UP) {
                        majinUI.setScene(SCENE_SETUP_WIFI);
                        majinVoice.playNotification();
                    }
                } 
                else {
                    // Mode Normal (Configuré)
                    
                    if (gesture == GESTURE_SWIPE_UP) { 
                        majinUI.setScene(SCENE_MENU); 
                        majinVoice.playNotification(); 
                    }
                    else if (gesture == GESTURE_SWIPE_DOWN) { 
                        majinUI.setScene(SCENE_TAMA_MENU); 
                        majinVoice.playNotification(); 
                    }
                    // 👇 MODIFICATION ICI : Le Tap ouvre les infos de config
                    else if (gesture == GESTURE_TAP) {
                        // Avant : Interaction (Bonjour)
                        // Maintenant : On affiche l'écran de configuration WiFi/Info
                        majinUI.setScene(SCENE_SETUP_WIFI); 
                        majinVoice.playNotification();
                    }
                }
            }
            
            // B. MENU TAMAGOTCHI
            else if (currentScene == SCENE_TAMA_MENU) {
                if (gesture == GESTURE_SWIPE_UP) { majinUI.setScene(SCENE_FACE); }
                else if (gesture == GESTURE_TAP) {
                    TamaAction action = majinUI.handleTamagotchiClick(touchX, touchY);
                    if (action == ACTION_FEED) { majinUI.showNotification("MIAM !", 0x07F3, 1000); majinVoice.playSuccess(); majinHead.setAngle(80); vTaskDelay(100); majinHead.setAngle(100); vTaskDelay(100); majinHead.setAngle(90); }
                    else if (action == ACTION_PLAY) { majinUI.showNotification("FUN +15", 0x079F, 1000); majinVoice.playSuccess(); for(int i=0;i<2;i++){majinHead.setAngle(70);vTaskDelay(100);majinHead.setAngle(110);vTaskDelay(100);} majinHead.setAngle(90); }
                    else if (action == ACTION_SLEEP) { majinUI.showNotification("BONNE NUIT...", 0x10A2, 2000); majinUI.setScene(SCENE_FACE); }
                    else if (action == ACTION_WAKE) { majinUI.showNotification("BONJOUR !", 0x079F, 1000); majinVoice.playSuccess(); majinUI.setScene(SCENE_FACE); }
                }
            }
            
            // C. SOUS-MENUS (Retour Accueil)
            else if (currentScene == SCENE_COUNTDOWN || currentScene == SCENE_SETUP_WIFI) {
                if (gesture == GESTURE_TAP || gesture == GESTURE_SWIPE_DOWN) { majinUI.setScene(SCENE_FACE); }
            }
            
            // D. MENU PRINCIPAL
            else if (currentScene == SCENE_MENU) {
                if (gesture == GESTURE_SWIPE_DOWN) { majinUI.setScene(SCENE_FACE); majinVoice.playNotification(); }
                else if (gesture == GESTURE_TAP) { if (majinUI.handleMenuClick(touchX, touchY)) majinVoice.playSuccess(); }
            }
            
            // E. APP TRACKPAD
            else if (currentScene == SCENE_APP_TRACKPAD) {
                if (gesture == GESTURE_SWIPE_DOWN) { majinUI.setScene(SCENE_MENU); majinVoice.playNotification(); }
                else if (gesture == GESTURE_TAP) { majinUI.showNotification("JIGGLING...", 0x079F, 1000); majinUSB.jiggleMouse(); }
            }
            
            // F. APP STREAM DECK
            else if (currentScene == SCENE_APP_STREAMDECK) {
                if (gesture == GESTURE_SWIPE_DOWN) { majinUI.setScene(SCENE_MENU); majinVoice.playNotification(); }
                // Navigation Pages
                else if (gesture == GESTURE_SWIPE_LEFT) { majinUI.nextDeckPage(); majinVoice.playNotification(); }
                else if (gesture == GESTURE_SWIPE_RIGHT) { majinUI.prevDeckPage(); majinVoice.playNotification(); }
                else if (gesture == GESTURE_TAP) {
                    // Logique Grille 3x2
                    int startX = 25; int startY = 50; int btnW = 80; int btnH = 60; int gap = 15;
                    int col = -1; int row = -1;
                    if (touchX > startX && touchX < startX + btnW) col = 0;
                    else if (touchX > startX + btnW + gap && touchX < startX + 2*btnW + gap) col = 1;
                    else if (touchX > startX + 2*(btnW + gap) && touchX < startX + 3*btnW + 2*gap) col = 2;
                    if (touchY > startY && touchY < startY + btnH) row = 0;
                    else if (touchY > startY + btnH + gap && touchY < startY + 2*btnH + gap) row = 1;

                    if (col != -1 && row != -1) {
                        int index = (majinUI.getDeckPage() * 6) + (row * 3 + col);
                        DeckButton btn = majinSettings.getDeckButton(index);
                        if (btn.active) {
                            majinUI.showNotification(String(btn.label), btn.color, 500);
                            majinUSB.sendShortcut(btn.key, btn.ctrl, btn.shift, btn.alt);
                            majinVoice.playSuccess();
                        } else {
                            majinUI.showNotification("VIDE", 0x5555, 500);
                        }
                    }
                }
            }
        }

        // 4. COMMANDES EXTERNES (Queue)
        if (xQueueReceive(guiQueue, &currentCmd, 0) == pdTRUE) {
            switch (currentCmd) {
                // Sortie de secours
                case CMD_HEAD_LONG_PRESS:
                    majinVoice.playNotification();
                    majinUI.setScene(SCENE_FACE);
                    majinUI.showNotification("RETOUR ACCUEIL", 0xF80A, 2000);
                    break;

                // Tap sur la tête
                case CMD_HEAD_TAP:
                    if (currentScene == SCENE_FACE || currentScene == SCENE_COUNTDOWN) {
                        String evt = majinSettings.getEventName();
                        unsigned long ts = majinSettings.getEventTimestamp();
                        // Si event -> Countdown, Sinon -> Bonjour
                        if (evt.length() > 0 && ts > 0) { majinUI.setScene(SCENE_COUNTDOWN); majinVoice.playSuccess(); }
                        else { majinVoice.playNotification(); majinUI.showNotification("BONJOUR !", 0x079F, 2000); majinHead.setAngle(45); vTaskDelay(200); majinHead.setAngle(90); }
                    }
                    break;

                case CMD_WIFI_CONNECTED: 
                    if (!majinSettings.isSetupDone()) { majinSettings.setSetupDone(true); String name = majinSettings.getRobotName(); majinUI.showNotification("BONJOUR " + name + " !", 0x07F3, 4000); majinUI.setScene(SCENE_FACE); }
                    else { majinUI.showNotification("WIFI CONNECTE", 0x07F3, 3000); }
                    majinVoice.playSuccess(); break;
                
                case CMD_WIFI_FAIL: majinUI.showNotification("ECHEC WIFI", 0xF986, 5000); majinVoice.playError(); break;
                
                case CMD_HELLO: majinVoice.playNotification(); majinUI.showNotification("BONJOUR !", 0x079F, 2000); if (majinUI.getScene() == SCENE_FACE) { majinHead.setAngle(45); vTaskDelay(200); majinHead.setAngle(90); } break;
                case CMD_DANCE: majinVoice.playNotification(); majinUI.showNotification("PARTY TIME!", 0xF80A, 4000); for(int i=0; i<3; i++) { majinHead.setAngle(60); vTaskDelay(150); majinHead.setAngle(120); vTaskDelay(150); } majinHead.setAngle(90); break;
                case CMD_MOOD_HAPPY: majinVoice.playSuccess(); break;
                case CMD_MOOD_ANGRY: majinVoice.playError(); majinHead.setAngle(70); vTaskDelay(100); majinHead.setAngle(90); break;
                default: break;
            }
        }

        // 5. RENDU GRAPHIQUE
        majinUI.update();
        majinUI.draw();
        
        // 6. CADENCEMENT (FPS)
        vTaskDelay(pdMS_TO_TICKS(5)); // ~Max FPS possible
    }
}