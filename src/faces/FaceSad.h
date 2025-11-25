#ifndef FACE_SAD_H
#define FACE_SAD_H
#include "FaceStructs.h"

class FaceSad {
private:
    static const int EYE_WIDTH = 96;
    static const int BLOCK_SIZE = 12;
public:
    static bool shouldDraw(FaceContext ctx) {
        int cx = 160; int cy = 120;
        int ey = cy - 10; 
        int lx = cx - 70; int rx = cx + 70;
        int h = (int)ctx.openHeight;

        // Yeux Tristes (Coupés en biais vers l'extérieur)
        if (_isSadEye(ctx.x, ctx.y, lx, ey, h, false)) return true;
        if (_isSadEye(ctx.x, ctx.y, rx, ey, h, true)) return true;

        // Bouche Triste
        int mY = 220; int tol = BLOCK_SIZE/2 + 2;
        bool top = (abs(ctx.y - (mY-15)) <= tol) && (abs(ctx.x - cx) < 40);
        bool sides = (abs(ctx.y - mY) <= tol) && (abs(ctx.x - cx) > 25) && (abs(ctx.x - cx) < 50);
        return top || sides;
    }
private:
    static bool _isSadEye(int px, int py, int ex, int ey, int h, bool right) {
        int w = EYE_WIDTH;
        if (px < ex-w/2 || px > ex+w/2 || py < ey-h/2 || py > ey+h/2) return false;
        
        // Coupe diagonale "triste" en haut
        // Gauche : pente descendante à gauche
        int slope = 30;
        float relX = (float)(px - ex);
        if (!right) { if (py < (ey - h/2 + slope) + (relX * 0.6)) return false; }
        else        { if (py < (ey - h/2 + slope) - (relX * 0.6)) return false; }
        
        return true;
    }
};
#endif