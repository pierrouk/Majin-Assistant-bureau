#include "BootSequence.h"
#include "../../include/Theme.h" 

// --- Partition : Axel F (Beverly Hills Cop) ---
#define NOTE_B3 247
#define NOTE_G4 392
#define NOTE_A4 440
#define NOTE_B4 494
#define NOTE_D5 587
#define NOTE_E5 659
#define P 0 

// Mélodie
const uint16_t MELODY[] = {
    NOTE_G4, NOTE_A4, NOTE_B4, NOTE_B3, NOTE_G4, NOTE_A4, 
    NOTE_B3, NOTE_A4, NOTE_G4, NOTE_B3, NOTE_D5,          
    NOTE_E5, NOTE_E5, NOTE_E5, NOTE_D5, NOTE_B3, NOTE_B3, 
    NOTE_G4, NOTE_A4, NOTE_B4, NOTE_B3, NOTE_A4,          
    NOTE_G4, NOTE_E5, NOTE_G4                             
};

// Durées
const uint16_t DURATIONS[] = {
    250, 250, 1500, 250, 250, 1500,
    375, 125, 1000, 250, 250,
    1500, 250, 125, 375, 1500, 250,
    125, 375, 1500, 250, 250,
    1000, 125, 1000
};

const int TOTAL_NOTES = sizeof(MELODY) / sizeof(MELODY[0]);

void BootSequence::_waitSafe(unsigned long ms, SoundSystem* voice) {
    unsigned long start = millis();
    while (millis() - start < ms) {
        voice->update(); 
        delay(5);        
    }
}

// 🎮 MOTEUR D'INTRO SYNCHRONISÉ
void BootSequence::_playIntroSync(LGFX_Majin* tft, SoundSystem* voice, ServoMotor* head) {
    int noteIndex = 0;
    
    float haloX = -50;       
    float haloSpeed = 6.0;   
    
    LGFX_Sprite textSprite(tft);
    textSprite.createSprite(320, 80); 
    textSprite.setTextSize(3); 
    textSprite.setTextDatum(middle_center);
    textSprite.setFont(FONT_TITLE); 

    // Boucle Principale
    while (noteIndex < TOTAL_NOTES || voice->isPlaying()) {
        
        voice->update();
        if (!voice->isPlaying() && noteIndex < TOTAL_NOTES) {
            uint16_t note = MELODY[noteIndex];
            uint16_t dur = DURATIONS[noteIndex];
            if (note > 0) voice->tone(note, dur); else voice->tone(0, dur);
            noteIndex++;
            if (noteIndex % 4 == 0) { 
                int target = (head->getAngle() > 90) ? 80 : 100;
                head->setAngle(target);
            }
        }

        // Effet Halo
        textSprite.fillScreen(COLOR_BG);
        textSprite.setTextColor(0x18E3); 
        textSprite.drawString("MAJIN OS", 160, 40);
        textSprite.setClipRect((int)haloX, 0, 60, 80); 
        textSprite.setTextColor(COLOR_PRIMARY); 
        textSprite.drawString("MAJIN OS", 160, 40);
        textSprite.clearClipRect(); 
        haloX += haloSpeed;
        if (haloX > 320) haloX = -60;
        textSprite.pushSprite(0, 80);
        delay(16); 
    }
    
    textSprite.deleteSprite();

    // --- FIN DE LA MUSIQUE : TRANSITION RÉVEIL ---
    
    tft->fillScreen(COLOR_BG); 
    head->setAngle(90); 

    // ⚠️ CORRECTION DIMENSIONS : Adaptation au style "Cercle EMO"
    int eyeWidth = EYE_RADIUS_MAX * 2;  // Largeur totale
    int eyeRadius = EYE_RADIUS_MAX;     // Rayon max (cercle)
    int eyeSpacing = 80;
    uint16_t color = COLOR_PRIMARY;
    
    int centerY = 120;
    int leftX = 160 - eyeSpacing;
    int rightX = 160 + eyeSpacing;

    // 1. Yeux fermés
    tft->fillRoundRect(leftX - eyeWidth/2, centerY, eyeWidth, 4, 2, color);
    tft->fillRoundRect(rightX - eyeWidth/2, centerY, eyeWidth, 4, 2, color);
    delay(800);

    // 2. "Groggy"
    for (int h = 4; h <= 20; h+=2) {
         tft->fillRect(leftX - eyeWidth/2, centerY - h/2 - 5, eyeWidth, h + 10, COLOR_BG);
         tft->fillRect(rightX - eyeWidth/2, centerY - h/2 - 5, eyeWidth, h + 10, COLOR_BG);
         
         // On adapte le rayon pour que ce soit plat quand c'est petit
         int r = (h < eyeRadius*2) ? h/2 : eyeRadius;
         tft->fillRoundRect(leftX - eyeWidth/2, centerY - h/2, eyeWidth, h, r, color);
         tft->fillRoundRect(rightX - eyeWidth/2, centerY - h/2, eyeWidth, h, r, color);
         delay(50); 
    }
    delay(400);

    // 3. Refermeture
    tft->fillRect(leftX - eyeWidth/2 - 5, centerY - 30, eyeWidth + 10, 60, COLOR_BG);
    tft->fillRect(rightX - eyeWidth/2 - 5, centerY - 30, eyeWidth + 10, 60, COLOR_BG);
    
    tft->fillRoundRect(leftX - eyeWidth/2, centerY, eyeWidth, 4, 2, color);
    tft->fillRoundRect(rightX - eyeWidth/2, centerY, eyeWidth, 4, 2, color);
    delay(600);

    // 4. "RÉVEIL !" (Ouverture complète)
    int targetH = EYE_RADIUS_MAX * 2; // Hauteur pleine (cercle)
    int maxH = targetH + 10; // Overshoot
    
    for (int h = 4; h <= maxH; h+=6) { 
         tft->fillRect(leftX - eyeWidth/2, centerY - h/2 - 5, eyeWidth, h + 10, COLOR_BG);
         tft->fillRect(rightX - eyeWidth/2, centerY - h/2 - 5, eyeWidth, h + 10, COLOR_BG);

         int r = (h < eyeWidth) ? h/2 : eyeRadius;
         tft->fillRoundRect(leftX - eyeWidth/2, centerY - h/2, eyeWidth, h, r, color);
         tft->fillRoundRect(rightX - eyeWidth/2, centerY - h/2, eyeWidth, h, r, color);
         delay(10); 
    }
    
    // Stabilisation
    tft->fillRect(leftX - eyeWidth/2 - 5, centerY - maxH/2 - 5, eyeWidth + 10, maxH + 10, COLOR_BG);
    tft->fillRect(rightX - eyeWidth/2 - 5, centerY - maxH/2 - 5, eyeWidth + 10, maxH + 10, COLOR_BG);
    
    tft->fillRoundRect(leftX - eyeWidth/2, centerY - targetH/2, eyeWidth, targetH, eyeRadius, color);
    tft->fillRoundRect(rightX - eyeWidth/2, centerY - targetH/2, eyeWidth, targetH, eyeRadius, color);

    // ✨ GLINT
    tft->fillCircle(leftX + 20, centerY - targetH/4, 12, EYE_GLINT_COLOR);
    tft->fillCircle(rightX + 20, centerY - targetH/4, 12, EYE_GLINT_COLOR);
    
    voice->playSuccess();
    _waitSafe(200, voice); 
}

void BootSequence::run(ServoMotor* head, SoundSystem* voice, DisplayDriver* display, 
                       SettingsManager* settings, UIManager* ui, 
                       CoreManager* core, NetworkManager* net) {
    // ⚠️ Attention : BootSequence ne connaît pas UsbManager.
    // C'est problématique car UIManager::begin en a besoin.
    // Solution : On passe NULL pour l'instant dans le boot car on n'utilise pas la souris pendant l'intro.
    // L'UI sera réinitialisée correctement dans TaskGUI avec le vrai pointeur.
    
    bool isFirstBoot = !settings->isSetupDone();
    if (!isFirstBoot) {
        voice->playStartup();
        _waitSafe(100, voice);
        uint8_t targetBrightness = settings->getScreenBrightness();
        for(int i=0; i<=targetBrightness; i+=10) { display->setBrightness(i); delay(5); }
        
        // ⚠️ ICI : nullptr pour usb
        ui->begin(display, core, net, settings, nullptr); 
        return;
    }

    Serial.println("🎬 [Boot]: Intro Halo...");
    display->setBrightness(200);
    LGFX_Majin* tft = display->getLGFX();
    tft->fillScreen(COLOR_BG);

    _playIntroSync(tft, voice, head);

    // ⚠️ ICI : nullptr pour usb
    ui->begin(display, core, net, settings, nullptr); 
    voice->mute();
    Serial.println("🎬 [Boot]: Fin.");
}