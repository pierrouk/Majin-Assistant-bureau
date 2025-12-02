#ifndef FACE_SAD_H
#define FACE_SAD_H
#include "FaceStructs.h"

class FaceSad {
public:
    static void draw(LGFX_Sprite* spr, FaceContext& ctx) {
        // Rectangle arrondi mais incliné vers l'extérieur ( /  \ )
        // On simule l'inclinaison par un trapèze ou juste un rectangle soft
        spr->fillRoundRect(ctx.x - ctx.w/2, ctx.y - ctx.h/2 + 5, ctx.w, ctx.h, 10, ctx.color);
        
        // Paupière lourde (Moitié supérieure coupée)
        // On dessine un rectangle noir en haut pour donner l'air fatigué
        spr->fillRect(ctx.x - ctx.w/2, ctx.y - ctx.h/2 - 5, ctx.w, ctx.h/2, TFT_BLACK);
    }
};
#endif