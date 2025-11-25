#include "FaceRenderer.h"
#include "../../include/Theme.h"

// Helpers Couleurs
uint16_t lightenColor(uint16_t color, uint8_t amount) {
    uint8_t r = (color >> 11) & 0x1F; uint8_t g = (color >> 5) & 0x3F; uint8_t b = color & 0x1F;
    r = (r + amount > 31) ? 31 : (r + amount); g = (g + amount*2 > 63) ? 63 : (g + amount*2); b = (b + amount > 31) ? 31 : (b + amount);
    return (r << 11) | (g << 5) | b;
}
uint16_t darkenColor(uint16_t color, uint8_t amount) {
    uint8_t r = (color >> 11) & 0x1F; uint8_t g = (color >> 5) & 0x3F; uint8_t b = color & 0x1F;
    r = (r < amount) ? 0 : (r - amount); g = (g < amount*2) ? 0 : (g - amount*2); b = (b < amount) ? 0 : (b - amount);
    return (r << 11) | (g << 5) | b;
}

FaceRenderer::FaceRenderer() {
}

void FaceRenderer::begin(CoreManager* core, SettingsManager* settings) {
    _core = core; _settings = settings;
}

void FaceRenderer::update() {
    int energy = _core->getEnergy();
    MoodState mood = _core->getMood();

    // Regard
    if (millis() - _lastGazeChange > (unsigned long)random(1500, 4000)) {
        _targetEyeX = random(-30, 30); _targetEyeY = random(-20, 20); _lastGazeChange = millis();
    }
    _currentEyeX += (_targetEyeX - _currentEyeX) * 0.1; 
    _currentEyeY += (_targetEyeY - _currentEyeY) * 0.1;

    // Respiration
    float breath = sin(millis() / 1000.0) * 5.0;

    // Clignement
    int maxOpen = 130; 
    if (energy < 20) maxOpen = 40; else if (energy < 40) maxOpen = 80;
    if (mood == MOOD_SLEEP) maxOpen = 10; 

    float targetH = maxOpen + breath;

    if (!_isBlinking) {
        int blinkDelay = (energy < 30) ? 4000 : 3000;
        if (millis() - _lastBlinkTime > (unsigned long)random(blinkDelay, blinkDelay * 1.5)) { _isBlinking = true; _lastBlinkTime = millis(); }
        _currentEyeHeight += (targetH - _currentEyeHeight) * 0.3;
    } else {
        unsigned long duration = millis() - _lastBlinkTime;
        if (duration < 80) { _currentEyeHeight = map(duration, 0, 80, targetH, 4); } 
        else if (duration < 160) { _currentEyeHeight = map(duration, 80, 160, 4, targetH); } 
        else { _isBlinking = false; _lastBlinkTime = millis(); }
    }
}

void FaceRenderer::draw(LGFX_Sprite* mainSprite) {
    mainSprite->fillScreen(0x0000);

    MoodState mood = _core->getMood();
    uint16_t brickColor = COLOR_PRIMARY; 
    if (mood == MOOD_ANGRY) brickColor = COLOR_DANGER; 
    if (mood == MOOD_SLEEP) brickColor = 0x2124; 

    FaceContext ctx;
    ctx.gazeX = _currentEyeX;
    ctx.gazeY = _currentEyeY;
    ctx.openHeight = _currentEyeHeight;
    ctx.mood = mood;

    int startX = 4; 

    for (int y = 0; y < 240; y += BLOCK_SIZE) {
        for (int x = startX; x < 320; x += BLOCK_SIZE) {
            
            ctx.x = x + BLOCK_SIZE/2;
            ctx.y = y + BLOCK_SIZE/2;

            bool drawBlock = false;
            // Routage Visages
            switch (mood) {
                case MOOD_HAPPY:   drawBlock = FaceHappy::shouldDraw(ctx); break;
                case MOOD_ANGRY:   drawBlock = FaceAngry::shouldDraw(ctx); break;
                case MOOD_SLEEP:   drawBlock = FaceSleep::shouldDraw(ctx); break;
                case MOOD_BORED:   drawBlock = FaceSad::shouldDraw(ctx); break;
                case MOOD_TIRED:   drawBlock = FaceSad::shouldDraw(ctx); break;
                case MOOD_HUNGRY:  drawBlock = FaceAngry::shouldDraw(ctx); break; 
                default:           drawBlock = FaceNormal::shouldDraw(ctx); break;
            }

            if (drawBlock) {
                _drawSingleBrick(mainSprite, x, y, brickColor);
            }
        }
    }
    
    int centerX = 160;
    _drawBubbles(mainSprite, centerX - 90 + _currentEyeX, centerX + 90 + _currentEyeX, 100 + _currentEyeY);
}

void FaceRenderer::_drawSingleBrick(LGFX_Sprite* spr, int x, int y, uint16_t mainColor) {
    int fullSize = BLOCK_SIZE - BLOCK_GAP;
    int radius = 1; 
    uint16_t lightColor = lightenColor(mainColor, 4);
    uint16_t darkColor = darkenColor(mainColor, 4);
    spr->fillRoundRect(x, y, fullSize, fullSize, radius, darkColor);
    spr->fillRoundRect(x, y, fullSize - 1, fullSize - 1, radius, lightColor);
    spr->fillRoundRect(x + 1, y + 1, fullSize - 2, fullSize - 2, 0, mainColor);
}

void FaceRenderer::_drawBubbles(LGFX_Sprite* spr, int leftX, int rightX, int centerY) {
    MoodState mood = _core->getMood();
    if (_core->getHunger() > 60) _drawHungerBubble(spr, rightX + 50, centerY - 60);
    if (_core->getEnergy() < 20 || mood == MOOD_SLEEP) _drawSleepBubble(spr, leftX - 50, centerY - 60);
}

void FaceRenderer::_drawHungerBubble(LGFX_Sprite* spr, int x, int y) {
    int size = 24; 
    spr->fillCircle(x, y, size, COLOR_WHITE); 
    spr->fillTriangle(x-10, y+size-5, x, y+size-5, x-15, y+size+10, COLOR_WHITE); 
    int bx = x - 10; int by = y - 5; 
    spr->fillRoundRect(bx, by, 20, 4, 2, 0xE463); 
    spr->fillRect(bx, by+5, 20, 2, 0x2580); 
    spr->fillRect(bx, by+8, 20, 3, 0xA145); 
    spr->fillRoundRect(bx, by+12, 20, 4, 2, 0xE463); 
}

void FaceRenderer::_drawSleepBubble(LGFX_Sprite* spr, int x, int y) {
    int size = 24; 
    spr->fillCircle(x, y, size, 0x10A2); 
    spr->fillTriangle(x+10, y+size-5, x, y+size-5, x+15, y+size+10, 0x10A2); 
    
    // ⚠️ Application de la police globale
    spr->setTextColor(COLOR_WHITE); 
    spr->setTextSize(1); 
    spr->setFont(FONT_UI); 
    spr->drawString("Zzz", x, y); 
}