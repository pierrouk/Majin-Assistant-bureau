#ifndef FACE_NORMAL_H
#define FACE_NORMAL_H

#include <Arduino.h>
#include "FaceStructs.h"

class FaceNormal {
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

        // Yeux
        if (_isInsideEye(ctx.x, ctx.y, leftX, eyeY, EYE_WIDTH, h)) 
            return !_isInsidePupil(ctx.x, ctx.y, leftX, eyeY, ctx);
        if (_isInsideEye(ctx.x, ctx.y, rightX, eyeY, EYE_WIDTH, h)) 
            return !_isInsidePupil(ctx.x, ctx.y, rightX, eyeY, ctx);

        // Bouche (Petite ligne)
        if (abs(ctx.y - 215) <= (BLOCK_SIZE/2 + 2) && abs(ctx.x - centerX) < 35) return true;

        return false;
    }

private:
    static bool _isInsideEye(int px, int py, int ex, int ey, int w, int h) {
        if (px < ex-w/2 || px > ex+w/2 || py < ey-h/2 || py > ey+h/2) return false;
        // Arrondi
        int dx = abs(px - ex); int dy = abs(py - ey); int c = 30;
        if (dx > w/2 - c && dy > h/2 - c && (dx-(w/2-c)) + (dy-(h/2-c)) > c) return false;
        return true;
    }
    static bool _isInsidePupil(int px, int py, int ex, int ey, FaceContext ctx) {
        int pSize = 18; 
        int paraX = (int)(ctx.gazeX * 0.5); int paraY = (int)(ctx.gazeY * 0.5);
        return (abs(px - (ex + paraX)) < pSize/2 && abs(py - (ey + paraY)) < pSize/2);
    }
};
#endif