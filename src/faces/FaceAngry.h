#ifndef FACE_ANGRY_H
#define FACE_ANGRY_H
#include "FaceStructs.h"

class FaceAngry {
public:
    static void draw(LGFX_Sprite* spr, FaceContext& ctx) {
        int w = ctx.w;
        int h = ctx.h;
        // Triangle agressif
        if (ctx.isLeft) {
            // Oeil Gauche : Incliné 
            spr->fillTriangle(ctx.x - w/2, ctx.y - h/2,     // Haut Gauche
                              ctx.x + w/2, ctx.y - h/2 + 15,// Haut Droite (plus bas)
                              ctx.x - w/2, ctx.y + h/2,     // Bas Gauche
                              ctx.color);
        } else {
            // Oeil Droit : Incliné 
            spr->fillTriangle(ctx.x - w/2, ctx.y - h/2 + 15,// Haut Gauche (plus bas)
                              ctx.x + w/2, ctx.y - h/2,     // Haut Droite
                              ctx.x + w/2, ctx.y + h/2,     // Bas Droite
                              ctx.color);
        }
        // Pas de pupille en colère, juste la forme lumineuse
    }
};
#endif