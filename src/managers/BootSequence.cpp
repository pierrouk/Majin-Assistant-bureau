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

const uint16_t MELODY[] = {
    NOTE_G4, NOTE_A4, NOTE_B4, NOTE_B3, NOTE_G4, NOTE_A4, 
    NOTE_B3, NOTE_A4, NOTE_G4, NOTE_B3, NOTE_D5,          
    NOTE_E5, NOTE_E5, NOTE_E5, NOTE_D5, NOTE_B3, NOTE_B3, 
    NOTE_G4, NOTE_A4, NOTE_B4, NOTE_B3, NOTE_A4,          
    NOTE_G4, NOTE_E5, NOTE_G4                             
};

const uint16_t DURATIONS[] = {
    250, 250, 1500, 250, 250, 1500,
    375, 125, 1000, 250, 250,
    1500, 250, 125, 375, 1500, 250,
    125, 375, 1500, 250, 250,
    1000, 125, 1000
};

const int TOTAL_NOTES = sizeof(MELODY) / sizeof(MELODY[0]);
const int SNOW_COUNT = 60; 

struct SnowFlake {
    float x; float y; float speed; float size;
};

void BootSequence::_waitSafe(unsigned long ms, SoundSystem* voice) {
    unsigned long start = millis();
    while (millis() - start < ms) { voice->update(); delay(5); }
}

uint16_t BootSequence::_rainbowColor(uint8_t index) {
    uint8_t region, remainder, p, q, t;
    uint8_t h = index; uint8_t s = 255; uint8_t v = 255;
    region = h / 43; remainder = (h - (region * 43)) * 6; 
    p = (v * (255 - s)) >> 8; q = (v * (255 - ((s * remainder) >> 8))) >> 8; t = (v * (255 - ((s * (255 - remainder)) >> 8))) >> 8;
    uint8_t r, g, b;
    switch (region) {
        case 0: r = v; g = t; b = p; break; case 1: r = q; g = v; b = p; break; case 2: r = p; g = v; b = t; break;
        case 3: r = p; g = q; b = v; break; case 4: r = t; g = p; b = v; break; default: r = v; g = p; b = q; break;
    }
    return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}

void BootSequence::_playIntroSync(LGFX_Majin* tft, SoundSystem* voice, ServoMotor* head) {
    int noteIndex = 0;
    int centerX = 160;
    int centerY = 120;

    SnowFlake snow[SNOW_COUNT];
    for(int i=0; i<SNOW_COUNT; i++) {
        snow[i].x = random(0, 320); snow[i].y = random(-240, 0); 
        snow[i].speed = random(10, 35) / 10.0; snow[i].size = random(1, 3);
    }

    while (noteIndex < TOTAL_NOTES || voice->isPlaying()) {
        voice->update();
        if (!voice->isPlaying() && noteIndex < TOTAL_NOTES) {
            uint16_t note = MELODY[noteIndex]; uint16_t dur = DURATIONS[noteIndex];
            if (note > 0) voice->tone(note, dur); else voice->tone(0, dur);
            noteIndex++;
            if (noteIndex % 4 == 0) { int target = (head->getAngle() > 90) ? 80 : 100; head->setAngle(target); }
        }

        tft->startWrite();
        tft->fillScreen(COLOR_BG);

        // Neige
        for(int i=0; i<SNOW_COUNT; i++) {
            snow[i].y += snow[i].speed; snow[i].x += sin(snow[i].y * 0.05) * 0.5;
            if (snow[i].y > 240) { snow[i].y = -5; snow[i].x = random(0, 320); }
            tft->fillCircle((int)snow[i].x, (int)snow[i].y, snow[i].size, COLOR_WHITE);
        }

        // Texte MAJIN OS
        tft->setFont(FONT_TITLE);
        tft->setTextSize(2.0); tft->setTextDatum(middle_center);
        String title = "MAJIN OS";
        int totalW = tft->textWidth(title);
        if (totalW > 300) { tft->setTextSize(1.5); totalW = tft->textWidth(title); }
        int cursorX = centerX - (totalW / 2);
        uint8_t baseHue = (millis() / 10) % 255;

        for (int i = 0; i < title.length(); i++) {
            tft->setTextColor(_rainbowColor(baseHue + (i * 15)));
            tft->setCursor(cursorX, centerY - 20); tft->print(title[i]);
            cursorX += tft->textWidth(title.substring(i, i+1));
        }

        // Sous-titre "Lets ride !!!"
        tft->setFont(FONT_UI); tft->setTextSize(1.5); tft->setTextColor(COLOR_WHITE);
        tft->drawString("Lets ride !!!", centerX, centerY + 40);
        
        // ⬅️ NOUVEAU : Affichage Version (Bas Droite)
        tft->setFont(FONT_SMALL); 
        tft->setTextSize(1); 
        tft->setTextColor(0x5555); // Gris discret
        tft->setTextDatum(bottom_right);
        tft->drawString(MAJIN_FW_VERSION, 315, 235);

        tft->endWrite();
    }
    
    // Transition Vectorielle (Reste inchangée)
    tft->fillScreen(COLOR_BG); head->setAngle(90); 
    int eyeW_Base = 55; int eyeH_Base = 85; int eyeSpacing = 35;
    tft->fillEllipse(centerX - eyeSpacing - eyeW_Base/2, 120, eyeW_Base/2, 2, COLOR_WHITE);
    tft->fillEllipse(centerX + eyeSpacing + eyeW_Base/2, 120, eyeW_Base/2, 2, COLOR_WHITE);
    delay(800);
    for (int h = 2; h <= eyeH_Base + 5; h+=4) { 
         tft->startWrite(); tft->fillScreen(COLOR_BG); 
         int currentW = eyeW_Base + (eyeH_Base - h) / 4;
         tft->fillEllipse(centerX - eyeSpacing - eyeW_Base/2, 120, currentW/2, h/2, COLOR_WHITE);
         tft->fillEllipse(centerX + eyeSpacing + eyeW_Base/2, 120, currentW/2, h/2, COLOR_WHITE);
         tft->endWrite(); delay(20); 
    }
    tft->fillScreen(COLOR_BG);
    int lx = centerX - eyeSpacing - eyeW_Base/2; int rx = centerX + eyeSpacing + eyeW_Base/2;
    tft->fillEllipse(lx, 120, eyeW_Base/2, eyeH_Base/2, COLOR_WHITE); tft->fillRect(lx - 6, 120 - 6, 12, 12, COLOR_BLACK); 
    tft->fillEllipse(rx, 120, eyeW_Base/2, eyeH_Base/2, COLOR_WHITE); tft->fillRect(rx - 6, 120 - 6, 12, 12, COLOR_BLACK); 
    voice->playSuccess(); _waitSafe(200, voice); 
}

void BootSequence::run(ServoMotor* head, SoundSystem* voice, DisplayDriver* display, 
                       SettingsManager* settings, UIManager* ui, CoreManager* core, NetworkManager* net) {
    if (settings->isSetupDone()) {
        display->setBrightness(settings->getScreenBrightness());
        ui->begin(display, core, net, settings, nullptr); return;
    }
    display->setBrightness(255); LGFX_Majin* tft = display->getLGFX();
    _playIntroSync(tft, voice, head);
    ui->begin(display, core, net, settings, nullptr); 
    voice->mute();
} 