#include "FaceRenderer.h"
#include "../../include/Theme.h"

// 📜 LISTE DES MESSAGES (Personnalisable ici)
const char* SPEECH_TEXTS[] = {
    "JAGER!!!!!!!",
    "Salut !",
    "Connecte-moi...",
    "Je suis Majin !",
    "Tape ma tete !",
    "J'attends...",
    "Lets Ride !"
};
const int SPEECH_COUNT = 6;

FaceRenderer::FaceRenderer() {}
void FaceRenderer::begin(CoreManager* core, SettingsManager* settings) { _core = core; _settings = settings; }

void FaceRenderer::update() {
    // 1. Hover
    _hoverPhase += 0.02; 
    _hoverOffsetX = sin(_hoverPhase * 0.7) * 30.0; _hoverOffsetY = cos(_hoverPhase) * 15.0;       

    // 2. Gaze
    if (millis() - _lastGazeChange > (unsigned long)random(800, 3000)) { 
        _targetGazeX = random(-20, 20); _targetGazeY = random(-10, 10); _lastGazeChange = millis(); 
    }
    _currentGazeX += (_targetGazeX - _currentGazeX) * 0.3; 
    _currentGazeY += (_targetGazeY - _currentGazeY) * 0.3;

    // 3. Blink
    if (!_isBlinking) {
        if (millis() - _lastBlinkTime > (unsigned long)random(2000, 5000)) { _isBlinking = true; _blinkProgress = 0.0;}
    } else {
        _blinkProgress += 0.25; 
        if (_blinkProgress >= 2.0) { _isBlinking = false; _lastBlinkTime = millis(); }
    }

    // 4. 💬 UPDATE SPEECH (Toutes les 10s)
    // On ne fait ça que si le setup n'est pas fait (Mode Attente)
    if (!_settings->isSetupDone()) {
        if (millis() - _lastSpeechChange > 10000) {
            _speechIndex = (_speechIndex + 1) % SPEECH_COUNT;
            _currentSpeech = String(SPEECH_TEXTS[_speechIndex]);
            _lastSpeechChange = millis();
        }
    } else {
        _currentSpeech = ""; // Pas de bulle en mode normal (pour l'instant)
    }
}

void FaceRenderer::draw(LGFX_Sprite* mainSprite) {
    mainSprite->fillScreen(TFT_BLACK);
    MoodState mood = _core->getMood();

    int cx = (320 / 2) + (int)_hoverOffsetX;
    int cy = (240 / 2) + (int)_hoverOffsetY;

    // Blink Factor
    float openFactor = 1.0;
    if (_isBlinking) {
        if (_blinkProgress <= 1.0) openFactor = 1.0 - _blinkProgress;
        else openFactor = _blinkProgress - 1.0;
    }
    if (mood == MOOD_SLEEP) openFactor = 0.05;

    // Contexte Visage
    FaceContext ctx;
    ctx.w = EYE_WIDTH_BASE; ctx.h = EYE_HEIGHT_BASE * openFactor; ctx.color = TFT_WHITE;
    ctx.gazeX = _currentGazeX; ctx.gazeY = _currentGazeY;

    // Oeil Gauche
    ctx.x = cx - EYE_SPACING - ctx.w/2; ctx.y = cy - 10; ctx.isLeft = true;
    switch(mood) {
        case MOOD_HAPPY: FaceHappy::draw(mainSprite, ctx); break;
        case MOOD_ANGRY: FaceAngry::draw(mainSprite, ctx); break;
        case MOOD_SAD:   FaceSad::draw(mainSprite, ctx); break;
        case MOOD_SLEEP: FaceSleep::draw(mainSprite, ctx); break;
        case MOOD_LOVE:  FaceLove::draw(mainSprite, ctx); break;
        default:         FaceNormal::draw(mainSprite, ctx); break;
    }

    // Oeil Droit
    ctx.x = cx + EYE_SPACING + ctx.w/2; ctx.isLeft = false;
    switch(mood) {
        case MOOD_HAPPY: FaceHappy::draw(mainSprite, ctx); break;
        case MOOD_ANGRY: FaceAngry::draw(mainSprite, ctx); break;
        case MOOD_SAD:   FaceSad::draw(mainSprite, ctx); break;
        case MOOD_SLEEP: FaceSleep::draw(mainSprite, ctx); break;
        case MOOD_LOVE:  FaceLove::draw(mainSprite, ctx); break;
        default:         FaceNormal::draw(mainSprite, ctx); break;
    }

    _drawRobotMouth(mainSprite, cx, cy + 40, mood);
    
    // 5. 💬 DESSIN DE LA BULLE (Si texte présent)
    if (_currentSpeech != "") {
        // Positionnée en haut à droite, fixe par rapport à l'écran (ne bouge pas avec la tête)
        _drawSpeechBubble(mainSprite, 220, 50, _currentSpeech);
    } else {
        // Sinon on dessine les icônes d'état (Faim/Sommeil)
        _drawBubbles(mainSprite, cx, cy - 70);
    }
}

void FaceRenderer::_drawSpeechBubble(LGFX_Sprite* spr, int x, int y, String text) {
    int w = 100; int h = 40;
    int r = 10;
    
    // Queue de la bulle (Vers la tête)
    spr->fillTriangle(x - 20, y + h/2, x, y + h/2 - 5, x, y + h/2 + 5, TFT_WHITE);
    
    // Corps de la bulle
    spr->fillRoundRect(x - w/2, y - h/2, w, h, r, TFT_WHITE);
    
    // Texte
    spr->setTextColor(TFT_BLACK);
    spr->setTextDatum(middle_center);
    spr->setFont(FONT_UI); // Assure-toi que c'est lisible
    spr->drawString(text, x, y);
}

void FaceRenderer::_drawRobotMouth(LGFX_Sprite* spr, int cx, int cy, MoodState mood) {
    uint16_t color = TFT_WHITE;
    switch (mood) {
        case MOOD_HAPPY: spr->fillArc(cx, cy - 10, 25, 22, 45, 135, color); break;
        case MOOD_ANGRY: spr->fillRect(cx - 10, cy, 20, 4, TFT_RED); break;
        case MOOD_SAD:   spr->fillArc(cx, cy + 10, 25, 22, 225, 315, color); break;
        case MOOD_SLEEP: spr->drawCircle(cx, cy, 5, color); break;
        case MOOD_LOVE:  spr->fillArc(cx, cy - 5, 15, 12, 45, 135, color); break;
        default:         spr->fillArc(cx, cy - 5, 20, 17, 60, 120, color); break;
    }
}

void FaceRenderer::_drawBubbles(LGFX_Sprite* spr, int cx, int topY) {
    if (_core->getHunger() > 80) _drawHungerBubble(spr, cx + 60, topY);
    if (_core->getEnergy() < 20 || _core->getMood() == MOOD_SLEEP) _drawSleepBubble(spr, cx - 60, topY);
}
void FaceRenderer::_drawHungerBubble(LGFX_Sprite* spr, int x, int y) {
    spr->fillCircle(x, y, 16, TFT_WHITE); spr->drawCircle(x, y, 16, TFT_RED); spr->fillRect(x-8, y-4, 16, 8, TFT_ORANGE);
}
void FaceRenderer::_drawSleepBubble(LGFX_Sprite* spr, int x, int y) {
    spr->fillCircle(x, y, 16, TFT_WHITE); spr->drawCircle(x, y, 16, TFT_BLUE); spr->setTextColor(TFT_BLUE, TFT_WHITE); spr->setTextDatum(middle_center); spr->drawString("Zzz", x, y);
}