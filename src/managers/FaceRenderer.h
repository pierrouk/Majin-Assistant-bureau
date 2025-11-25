#ifndef FACE_RENDERER_H
#define FACE_RENDERER_H

#include <Arduino.h>
#include <LovyanGFX.hpp>
#include "CoreManager.h"
#include "SettingsManager.h"

// ⚠️ INCLUSIONS OBLIGATOIRES DES VISAGES
#include "../faces/FaceNormal.h"
#include "../faces/FaceHappy.h"
#include "../faces/FaceAngry.h"
#include "../faces/FaceSleep.h"
#include "../faces/FaceLove.h"
#include "../faces/FaceSad.h"

// 👾 CONFIGURATION GRILLE
#define BLOCK_SIZE 12    
#define BLOCK_GAP  1     

class FaceRenderer {
public:
    FaceRenderer();

    void begin(CoreManager* core, SettingsManager* settings);
    void update();
    void draw(LGFX_Sprite* mainSprite);

private:
    CoreManager* _core;
    SettingsManager* _settings;

    // Variables d'Animation
    unsigned long _lastBlinkTime = 0;
    bool _isBlinking = false;
    
    float _currentEyeX = 0; 
    float _currentEyeY = 0; 
    float _targetEyeX = 0; 
    float _targetEyeY = 0;
    unsigned long _lastGazeChange = 0;
    
    float _currentEyeHeight = 120; 

    // Helper Dessin Brique
    void _drawSingleBrick(LGFX_Sprite* spr, int x, int y, uint16_t color);
    
    // Bulles
    void _drawBubbles(LGFX_Sprite* spr, int leftX, int rightX, int centerY);
    void _drawHungerBubble(LGFX_Sprite* spr, int x, int y);
    void _drawSleepBubble(LGFX_Sprite* spr, int x, int y);
};

#endif