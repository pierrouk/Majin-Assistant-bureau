#ifndef FACE_STRUCTS_H
#define FACE_STRUCTS_H

#include <Arduino.h>
#include <LovyanGFX.hpp>

// Structure pour transmettre l'état physique du visage aux fonctions de dessin
struct FaceContext {
    int x;          // Position Centre X de l'oeil
    int y;          // Position Centre Y de l'oeil
    int w;          // Largeur actuelle (Scale)
    int h;          // Hauteur actuelle (Scale + Clignement)
    uint16_t color; // Couleur de l'oeil
    float gazeX;    // Décalage Pupille X
    float gazeY;    // Décalage Pupille Y
    bool isLeft;    // Est-ce l'oeil gauche ? (Pour les asymétries comme Angry)
};

#endif