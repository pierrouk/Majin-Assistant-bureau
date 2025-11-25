#ifndef FACE_HAPPY_H
#define FACE_HAPPY_H

#include <Arduino.h>
#include "FaceStructs.h"

class FaceHappy {
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

        // Yeux qui sourient (Pas de pupille, c'est plus mignon)
        if (_isInsideHappyEye(ctx.x, ctx.y, leftX, eyeY, EYE_WIDTH, h)) return true;
        if (_isInsideHappyEye(ctx.x, ctx.y, rightX, eyeY, EYE_WIDTH, h)) return true;

        // Bouche Sourire
        int mY = 215; int tol = BLOCK_SIZE/2 + 2;
        bool bot = (abs(ctx.y - mY) <= tol) && (abs(ctx.x - centerX) < 45);
        bool sides = (abs(ctx.y - (mY-18)) <= tol) && (abs(ctx.x - centerX) > 30) && (abs(ctx.x - centerX) < 60);
        return bot || sides;
    }

private:
    static bool _isInsideHappyEye(int px, int py, int ex, int ey, int w, int h) {
        if (px < ex-w/2 || px > ex+w/2 || py < ey-h/2 || py > ey+h/2) return false;
        // Arrondi haut
        int dx = abs(px - ex); int dy = abs(py - ey); int c = 30;
        if (dy < 0 && dx > w/2 - c && dy > h/2 - c && (dx-(w/2-c)) + (dy-(h/2-c)) > c) return false;

        // Masque bas (Sourire joue) - Coupe parabolique
        float relX = (float)(px - ex) / (w/2.0);
        float curveH = h * 0.5;
        if (py > (ey + h/2) - (1.0 - relX*relX) * curveH) return false;

        return true;
    }
};
#endif