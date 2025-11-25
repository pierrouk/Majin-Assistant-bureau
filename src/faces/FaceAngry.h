#ifndef FACE_ANGRY_H
#define FACE_ANGRY_H

#include <Arduino.h>
#include "FaceStructs.h"

class FaceAngry {
private:
    static const int EYE_WIDTH = 96;
    static const int EYE_SPACING = 140;
    static const int BLOCK_SIZE = 12;

public:
    static bool shouldDraw(FaceContext ctx) {
        int centerX = 160; int centerY = 120;
        int eyeY = centerY - 20 + (int)ctx.gazeY;
        int h = (int)ctx.openHeight;
        int leftX = centerX - (EYE_SPACING / 2) + (int)ctx.gazeX;
        int rightX = centerX + (EYE_SPACING / 2) + (int)ctx.gazeX;

        // Yeux coupés
        if (_isInsideAngryEye(ctx.x, ctx.y, leftX, eyeY, EYE_WIDTH, h)) 
            return !_isInsidePupil(ctx.x, ctx.y, leftX, eyeY, ctx);
        if (_isInsideAngryEye(ctx.x, ctx.y, rightX, eyeY, EYE_WIDTH, h)) 
            return !_isInsidePupil(ctx.x, ctx.y, rightX, eyeY, ctx);

        // Bouche Fâchée (U inversé)
        int mY = 215; int tol = BLOCK_SIZE/2 + 2;
        bool top = (abs(ctx.y - (mY-18)) <= tol) && (abs(ctx.x - centerX) < 45);
        bool sides = (abs(ctx.y - mY) <= tol) && (abs(ctx.x - centerX) > 30) && (abs(ctx.x - centerX) < 60);
        return top || sides;
    }

private:
    static bool _isInsideAngryEye(int px, int py, int ex, int ey, int w, int h) {
        if (px < ex-w/2 || px > ex+w/2 || py < ey-h/2 || py > ey+h/2) return false;
        
        // Pente sourcil
        int slope = 40;
        float relX = (float)(px - ex);
        bool isRight = (ex > 160);
        if (isRight) { if (py < (ey - h/2 + slope) - (relX * 0.5)) return false; }
        else { if (py < (ey - h/2 + slope) + (relX * 0.5)) return false; }
        
        return true;
    }
    static bool _isInsidePupil(int px, int py, int ex, int ey, FaceContext ctx) {
        int pSize = 18; 
        int paraX = (int)(ctx.gazeX * 0.5); int paraY = (int)(ctx.gazeY * 0.5);
        return (abs(px - (ex + paraX)) < pSize/2 && abs(py - (ey + paraY)) < pSize/2);
    }
};
#endif