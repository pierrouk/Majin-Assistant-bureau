#ifndef FACE_RENDERER_H
#define FACE_RENDERER_H

#include <Arduino.h>
#include <LovyanGFX.hpp>
#include "../managers/CoreManager.h"
#include "../managers/SettingsManager.h"
#include "../faces/FaceStructs.h"

// Inclusions modulaires
#include "../faces/FaceNormal.h"
#include "../faces/FaceHappy.h"
#include "../faces/FaceAngry.h"
#include "../faces/FaceSleep.h"
#include "../faces/FaceLove.h"
#include "../faces/FaceSad.h"

class FaceRenderer {
public:
    FaceRenderer();
    void begin(CoreManager* core, SettingsManager* settings);
    void update();
    void draw(LGFX_Sprite* mainSprite);

private:
    CoreManager* _core;
    SettingsManager* _settings;

    const int EYE_WIDTH_BASE = 50; const int EYE_HEIGHT_BASE = 80; const int EYE_SPACING = 35;      

    // Animation Visage
    unsigned long _lastBlinkTime = 0;
    bool _isBlinking = false;
    float _blinkProgress = 0.0;
    float _currentGazeX = 0; float _currentGazeY = 0; 
    float _targetGazeX = 0; float _targetGazeY = 0;
    unsigned long _lastGazeChange = 0;
    float _hoverPhase = 0.0;
    float _hoverOffsetY = 0.0; float _hoverOffsetX = 0.0;

    // 💬 SYSTEME DE BULLE (Speech Bubble)
    unsigned long _lastSpeechChange = 0;
    int _speechIndex = 0;
    String _currentSpeech = "";
    
    // Helpers
    void _drawRobotMouth(LGFX_Sprite* spr, int cx, int cy, MoodState mood);
    void _drawBubbles(LGFX_Sprite* spr, int cx, int topY);
    void _drawHungerBubble(LGFX_Sprite* spr, int x, int y);
    void _drawSleepBubble(LGFX_Sprite* spr, int x, int y);
    
    // Nouvelle fonction
    void _drawSpeechBubble(LGFX_Sprite* spr, int x, int y, String text);
};
#endif