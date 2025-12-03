#ifndef FACE_SLEEP_H
#define FACE_SLEEP_H
#include "FaceStructs.h"

class FaceSleep {
public:
    static void draw(LGFX_Sprite* spr, FaceContext& ctx) {
        // Petit trait horizontal (Yeux fermés détendus)
        spr->fillRoundRect(ctx.x - ctx.w/2, ctx.y, ctx.w, 4, 2, ctx.color);
    }
};
#endif