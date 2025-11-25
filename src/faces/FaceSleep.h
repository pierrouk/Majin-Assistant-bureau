#ifndef FACE_SLEEP_H
#define FACE_SLEEP_H

#include <Arduino.h>
#include "FaceStructs.h"

class FaceSleep {
private:
    static const int EYE_SPACING = 140;
    static const int BLOCK_SIZE = 12;

public:
    static bool shouldDraw(FaceContext ctx) {
        int centerX = 160; int centerY = 120;
        int eyeY = centerY - 10; // Yeux un peu plus bas quand on dort
        
        // Les yeux ne suivent pas le regard quand on dort
        int leftX = centerX - (EYE_SPACING / 2);
        int rightX = centerX + (EYE_SPACING / 2);

        // Yeux = Traits horizontaux simples
        int eyeW = 80;
        int tol = BLOCK_SIZE/2 + 1;
        
        if (abs(ctx.y - eyeY) <= tol) {
            if (abs(ctx.x - leftX) < eyeW/2) return true;
            if (abs(ctx.x - rightX) < eyeW/2) return true;
        }

        // Bouche "o" (Ronflement)
        int mY = 220;
        int dx = abs(ctx.x - centerX);
        int dy = abs(ctx.y - mY);
        // Un petit carré vide
        if (dx < 15 && dy < 15) {
            if (dx > 5 || dy > 5) return true; // Bordure seulement
        }

        return false;
    }
};
#endif