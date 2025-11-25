#ifndef FACE_LOVE_H
#define FACE_LOVE_H

#include <Arduino.h>
#include "FaceStructs.h"

class FaceLove {
private:
    static const int EYE_SPACING = 140;
    static const int BLOCK_SIZE = 12;

public:
    static bool shouldDraw(FaceContext ctx) {
        int centerX = 160; int centerY = 120;
        int eyeY = centerY - 20 + (int)ctx.gazeY;
        int leftX = centerX - (EYE_SPACING / 2) + (int)ctx.gazeX;
        int rightX = centerX + (EYE_SPACING / 2) + (int)ctx.gazeX;

        // Yeux Coeurs
        if (_isInsideHeart(ctx.x, ctx.y, leftX, eyeY)) return true;
        if (_isInsideHeart(ctx.x, ctx.y, rightX, eyeY)) return true;

        // Bouche Sourire
        int mY = 215; int tol = BLOCK_SIZE/2 + 2; 
        bool bot = (abs(ctx.y - mY) <= tol) && (abs(ctx.x - centerX) < 40);
        bool sides = (abs(ctx.y - (mY-15)) <= tol) && (abs(ctx.x - centerX) > 25) && (abs(ctx.x - centerX) < 50);
        return bot || sides;
    }

private:
    static bool _isInsideHeart(int px, int py, int cx, int cy) {
        // Équation simple du coeur ou assemblage géométrique
        // 2 cercles en haut + 1 triangle en bas
        int r = 25;
        int d = r * 0.8;
        
        // Cercles du haut
        int dxL = px - (cx - d); int dyTop = py - (cy - 10);
        if (dxL*dxL + dyTop*dyTop < r*r) return true;
        
        int dxR = px - (cx + d);
        if (dxR*dxR + dyTop*dyTop < r*r) return true;
        
        // Triangle bas
        // Pointe vers le bas (cy + 50)
        // Largeur en haut (cy)
        if (py >= (cy - 10) && py < (cy + 50)) {
            int halfW = map(py, cy - 10, cy + 50, 50, 0);
            if (abs(px - cx) < halfW) return true;
        }
        return false;
    }
};
#endif