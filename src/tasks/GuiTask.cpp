#include "../Globals.h"
#include "GuiTask.h"

void TaskGUI(void *pvParameters) {
    Serial.println("🎨 [GuiTask] Démarrage Core 1 (Fast Boot)");

    // 1. Initialisation de l'écran
    if (!majinScreen.begin()) { 
        Serial.println("❌ GUI: Erreur Ecran"); 
    } else { 
        // Initialisation du Manager UI
        // On passe les pointeurs globaux définis dans Globals.h
        majinUI.begin(&majinScreen, &majinCore, &majinNet, &majinSettings, &majinUSB); 
    }
    
    // 2. Nettoyage écran (Fond noir de base)
    majinScreen.secureDraw([](LGFX_Majin* tft) { tft->fillScreen(0x0000); }); 

    // 3. LOGIQUE FAST BOOT (Sprint 3)
    // Si le robot est déjà configuré, on affiche immédiatement le visage
    // sans attendre le WiFi ou l'intro.
    if (majinSettings.isSetupDone()) {
        majinUI.setScene(SCENE_FACE);
        // Petit son discret pour dire "Je suis là" (pas la mélodie complète)
        majinVoice.playNotification(); 
    } else {
        // Si pas configuré, on laisse l'écran par défaut (Boot) 
        // ou on attend que la BootSequence (dans le main) finisse.
    }

    RobotCommand currentCmd;
    uint16_t touchX = 0, touchY = 0;
    bool isTouched = false;
    
    // Variables pour le mouvement "Idle" (Vivant)
    unsigned long lastIdleMove = 0;
    unsigned long nextIdleMoveDelay = 3000; 

    // Boucle Infinie
    while (true) {
        // ---------------------------------------------------------
        // 1. ACQUISITION TACTILE & INPUT
        // ---------------------------------------------------------
        isTouched = majinScreen.getTouchSafe(&touchX, &touchY);
        
        // FEEDBACK HAPTIQUE (Vibreur au toucher)
        static bool wasTouched = false;
        if (isTouched && !wasTouched) {
            majinHaptic.click(); 
        }
        wasTouched = isTouched;

        // Traitement du geste (Tap, Swipe, Long Press...)
        GestureType gesture = majinInput.process(isTouched, touchX, touchY);
        UIScene currentScene = majinUI.getScene();

        // ---------------------------------------------------------
        // 2. COMPORTEMENT IDLE (Si on est sur le visage)
        // ---------------------------------------------------------
        if (currentScene == SCENE_FACE) {
             if (millis() - lastIdleMove > nextIdleMoveDelay) {
                lastIdleMove = millis();
                nextIdleMoveDelay = random(2000, 8000); 
                // Petit mouvement de tête aléatoire pour donner de la vie
                majinHead.setAngle(random(70, 110));
            }
        }

        // ---------------------------------------------------------
        // 3. GESTION DES GESTES (Navigation)
        // ---------------------------------------------------------
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
                    else if (gesture == GESTURE_TAP) {
                        // Accès rapide aux Infos Réseau / Config via Tap
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
                    if (action == ACTION_FEED) { 
                        majinUI.showNotification("MIAM !", 0x07F3, 1000); 
                        majinVoice.playSuccess(); 
                        // Petite animation de tête "Miam"
                        majinHead.setAngle(80); vTaskDelay(100); majinHead.setAngle(100); vTaskDelay(100); majinHead.setAngle(90); 
                    }
                    else if (action == ACTION_PLAY) { 
                        majinUI.showNotification("FUN +15", 0x079F, 1000); 
                        majinVoice.playSuccess(); 
                        // Animation de tête "Non non" rapide (Jeu)
                        for(int i=0;i<2;i++){majinHead.setAngle(70);vTaskDelay(100);majinHead.setAngle(110);vTaskDelay(100);} majinHead.setAngle(90); 
                    }
                    else if (action == ACTION_SLEEP) { 
                        majinUI.showNotification("BONNE NUIT...", 0x10A2, 2000); 
                        majinUI.setScene(SCENE_FACE); 
                    }
                    else if (action == ACTION_WAKE) { 
                        majinUI.showNotification("BONJOUR !", 0x079F, 1000); 
                        majinVoice.playSuccess(); 
                        majinUI.setScene(SCENE_FACE); 
                    }
                }
            }
            
            // C. SOUS-MENUS (Retour Accueil)
            else if (currentScene == SCENE_COUNTDOWN || currentScene == SCENE_SETUP_WIFI) {
                if (gesture == GESTURE_TAP || gesture == GESTURE_SWIPE_DOWN) { majinUI.setScene(SCENE_FACE); }
            }
            
            // D. MENU PRINCIPAL (Hexagonal)
            else if (currentScene == SCENE_MENU) {
                // Retour accueil
                if (gesture == GESTURE_SWIPE_DOWN) { 
                    majinUI.setScene(SCENE_FACE); 
                    majinVoice.playNotification(); 
                }
                // Scroll Gauche/Droite
                else if (gesture == GESTURE_SWIPE_LEFT) {
                    majinUI.nextDeckPage(); // Utilise la même logique interne ou accède direct
                    // Astuce : UIManager n'expose pas menuTarget en public, 
                    // mais on peut simuler un clic droit ou ajouter une methode.
                    // Pour faire simple ici, on va modifier UIManager.h pour ajouter nextMenu() / prevMenu()
                    // OU plus simple : on simule un clic
                    majinUI.handleMenuClick(300, 120); // Simule un tap à droite
                }
                else if (gesture == GESTURE_SWIPE_RIGHT) {
                    majinUI.handleMenuClick(20, 120); // Simule un tap à gauche
                }
                // Validation
                else if (gesture == GESTURE_TAP) { 
                    if (majinUI.handleMenuClick(touchX, touchY)) majinVoice.playSuccess(); 
                }
            }
            
            // E. APP TRACKPAD (Souris)
            else if (currentScene == SCENE_APP_TRACKPAD) {
                if (gesture == GESTURE_SWIPE_DOWN) { majinUI.setScene(SCENE_MENU); majinVoice.playNotification(); }
                else if (gesture == GESTURE_TAP) { 
                    majinUI.showNotification("JIGGLING...", 0x079F, 1000); 
                    majinUSB.jiggleMouse(); 
                }
            }
            
            // F. APP STREAM DECK
            else if (currentScene == SCENE_APP_STREAMDECK) {
                if (gesture == GESTURE_SWIPE_DOWN) { majinUI.setScene(SCENE_MENU); majinVoice.playNotification(); }
                // Navigation Pages
                else if (gesture == GESTURE_SWIPE_LEFT) { majinUI.nextDeckPage(); majinVoice.playNotification(); }
                else if (gesture == GESTURE_SWIPE_RIGHT) { majinUI.prevDeckPage(); majinVoice.playNotification(); }
                
                // 👇 C'EST ICI QUE CA CHANGE (Beaucoup plus propre !)
                else if (gesture == GESTURE_TAP) {
                    // On demande au Manager : "Sur quel bouton j'ai appuyé ?"
                    int btnIndex = majinUI.handleDeckClick(touchX, touchY);
                    
                    if (btnIndex != -1) {
                        // Un bouton a été trouvé !
                        DeckButton btn = majinSettings.getDeckButton(btnIndex);
                        
                        // Envoi du raccourci clavier
                        majinUSB.sendShortcut(btn.key, btn.ctrl, btn.shift, btn.alt);
                        
                        // Feedback sonore
                        majinVoice.playSuccess();
                    }
                }
            }
        }

        // ---------------------------------------------------------
        // 4. COMMANDES EXTERNES (Queue System)
        // ---------------------------------------------------------
        if (xQueueReceive(guiQueue, &currentCmd, 0) == pdTRUE) {
            switch (currentCmd) {
                // Sortie de secours
                case CMD_HEAD_LONG_PRESS:
                    majinVoice.playNotification();
                    majinUI.setScene(SCENE_FACE);
                    majinUI.showNotification("RETOUR ACCUEIL", 0xF80A, 2000);
                    break;

                // Tap sur la tête (Capteur Capacitif)
                case CMD_HEAD_TAP:
                    if (currentScene == SCENE_FACE || currentScene == SCENE_COUNTDOWN) {
                        String evt = majinSettings.getEventName();
                        unsigned long ts = majinSettings.getEventTimestamp();
                        // Si event -> Countdown, Sinon -> Bonjour
                        if (evt.length() > 0 && ts > 0) { 
                            majinUI.setScene(SCENE_COUNTDOWN); 
                            majinVoice.playSuccess(); 
                        }
                        else { 
                            majinVoice.playNotification(); 
                            majinUI.showNotification("BONJOUR !", 0x079F, 2000); 
                            majinHead.setAngle(45); vTaskDelay(200); majinHead.setAngle(90); 
                        }
                    }
                    break;

                case CMD_WIFI_CONNECTED: 
                    if (!majinSettings.isSetupDone()) { 
                        // Fin du Setup
                        majinSettings.setSetupDone(true); 
                        String name = majinSettings.getRobotName(); 
majinVoice.playSuccess(); // R2D2 Content
                        majinUI.setScene(SCENE_FACE); 
                    }
                    else { 
                        majinUI.showNotification("WIFI CONNECTE", 0x07F3, 3000); 
                    }
                    majinVoice.playSuccess(); 
                    break;
                
                case CMD_WIFI_FAIL: 
                    majinUI.showNotification("ECHEC WIFI", 0xF986, 5000); 
                    majinVoice.playError(); 
                    break;
                
                case CMD_HELLO: 
                    majinVoice.playHappy(); // Son R2D2
                    majinUI.showNotification("BONJOUR !", 0x079F, 2000); 
                    majinHead.animHappy(); // ⬅️ Nouvelle animation
                    break;

                case CMD_DANCE: 
                    majinVoice.playSuccess();
                    majinUI.showNotification("PARTY TIME!", 0xF80A, 4000); 
                    // Danse custom
                    majinHead.shake(4, 30, 150); 
                    break;

                case CMD_MOOD_HAPPY: 
                    majinVoice.playHappy(); 
                    majinHead.animHappy(); // ⬅️ Hop !
                    break;

                case CMD_MOOD_ANGRY: 
                    majinVoice.playAngry(); 
                    majinHead.animAngry(); // ⬅️ Hop !
                    break;
                
                case CMD_MOOD_SAD:    // Ajoute ces cas si tu les as dans ton Enum RobotCommand
                case CMD_MOOD_TIRED:
                    majinVoice.playSad();
                    majinHead.animSad();
                    break;
                default: break;
            }
        }

        // ---------------------------------------------------------
        // 5. RENDU GRAPHIQUE
        // ---------------------------------------------------------
        majinUI.update();
        majinUI.draw();
        
        // ---------------------------------------------------------
        // 6. CADENCEMENT (FPS)
        // ---------------------------------------------------------
        vTaskDelay(pdMS_TO_TICKS(5)); // ~200 FPS théorique max, limité par le SPI
    }
}