#ifndef THEME_H
#define THEME_H

#include <Arduino.h>
#include <LovyanGFX.hpp>

// --- 🎨 PALETTE DE COULEURS (RGB565) ---
#define COLOR_PRIMARY     0x079F // Cyan (Reste du thème)
#define COLOR_ACCENT      0xF80A // Rose
#define COLOR_SUCCESS     0x07F3 // Vert
#define COLOR_DANGER      0xF986 // Rouge
#define COLOR_WARNING     0xFD20 // Orange (Nouveau)

#define COLOR_WHITE       0xFFFF
#define COLOR_BLACK       0x0000 
#define COLOR_BG          0x0000 
#define COLOR_UI_BG       0x2124 

// Couleurs spécifiques Widgets (Tamagotchi)
#define COLOR_ORANGE      0xFDA0
#define COLOR_BROWN       0x9280
#define COLOR_BLUE        0x001F

// --- 🔗 COMPATIBILITÉ (Mapping pour le FaceRenderer) ---
// Permet d'utiliser les noms standards LovyanGFX dans le renderer
#define TFT_WHITE   COLOR_WHITE
#define TFT_BLACK   COLOR_BLACK
#define TFT_RED     COLOR_DANGER
#define TFT_ORANGE  COLOR_ORANGE
#define TFT_BROWN   COLOR_BROWN
#define TFT_BLUE    COLOR_BLUE

// --- 👁️ DESIGN ---
// Note: La taille est maintenant gérée dynamiquement dans FaceRenderer.h
// Mais on garde la couleur ici pour pouvoir la changer facilement via le Web plus tard.
#define EYE_COLOR         COLOR_WHITE  // 👈 Mis à jour en BLANC (Sprint 1)

// --- ✒️ POLICES ---
#define FONT_TITLE        &fonts::Font4
#define FONT_SMALL        &fonts::Font0
#define FONT_UI           &fonts::Font2

#endif