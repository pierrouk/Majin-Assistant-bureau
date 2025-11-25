#ifndef THEME_H
#define THEME_H

#include <Arduino.h>
#include <LovyanGFX.hpp>

// --- 🎨 PALETTE DE COULEURS (RGB565) ---
#define COLOR_PRIMARY     0x079F // Cyan
#define COLOR_ACCENT      0xF80A // Rose
#define COLOR_SUCCESS     0x07F3 // Vert
#define COLOR_DANGER      0xF986 // Rouge
#define COLOR_WHITE       0xFFFF
#define COLOR_BLACK       0x0000 
#define COLOR_BG          0x0000 
#define COLOR_UI_BG       0x2124 

// --- 👁️ DESIGN ---
#define EYE_RADIUS_MAX    55    
#define EYE_COLOR         COLOR_PRIMARY
#define EYE_GLINT_COLOR   COLOR_WHITE
#define EYE_HEIGHT_OPEN   130
#define EYE_CORNER_RADIUS 45

// --- ✒️ POLICES PIXEL ART (GLCD) ---
// Ces polices sont "pixel perfect" et ne font pas de flou.
// Font0 = 6px, Font2 = 16px, Font4 = 26px, Font7 = 48px (7-seg)

#define FONT_TITLE        &fonts::Font4  // Gros titres (Style Arcade)
#define FONT_SMALL        &fonts::Font0  // Petits détails (Style Terminal)
#define FONT_UI           &fonts::Font2  // Menus et boutons (Lisible)

#endif