#ifndef FACE_NORMAL_H
#define FACE_NORMAL_H
#include "FaceStructs.h"

class FaceNormal {
public:
    static void draw(LGFX_Sprite* spr, FaceContext& ctx) {
        // Forme de base (Ovale "Pill Shape")
        if (ctx.h < 4) { spr->fillRect(ctx.x - ctx.w/2, ctx.y, ctx.w, 3, ctx.color); return; }
        
        spr->fillRoundRect(ctx.x - ctx.w/2, ctx.y - ctx.h/2, ctx.w, ctx.h, ctx.w/2, ctx.color);
        
        // Pupille Digitale (Carrée pour le style Robot)
        // On ne la dessine que si l'oeil est assez ouvert
        if (ctx.h > 20) {
            int px = ctx.x + ctx.gazeX;
            int py = ctx.y + ctx.gazeY;
            spr->fillRect(px - 6, py - 6, 12, 12, TFT_BLACK); 
        }
    }
};
#endif