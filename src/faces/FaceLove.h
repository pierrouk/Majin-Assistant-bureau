#ifndef FACE_LOVE_H
#define FACE_LOVE_H
#include "FaceStructs.h"

class FaceLove {
public:
    static void draw(LGFX_Sprite* spr, FaceContext& ctx) {
        // Yeux en Coeur <3
        // On triche avec deux cercles et un triangle
        int r = ctx.w / 4;
        int x = ctx.x;
        int y = ctx.y - 5;
        
        // Haut du coeur (2 cercles)
        spr->fillCircle(x - r, y, r, ctx.color);
        spr->fillCircle(x + r, y, r, ctx.color);
        
        // Bas du coeur (Triangle inversé)
        spr->fillTriangle(x - r*2, y + r/2, 
                          x + r*2, y + r/2, 
                          x, y + r*3, 
                          ctx.color);
    }
};
#endif