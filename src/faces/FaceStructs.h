#ifndef FACE_STRUCTS_H
#define FACE_STRUCTS_H

#include <Arduino.h>
#include "../managers/CoreManager.h" // Pour MoodState

// Ce que le moteur envoie au visage à chaque frame
struct FaceContext {
    int x;              // Position actuelle du pixel (Grille)
    int y;
    float gazeX;        // Décalage regard X
    float gazeY;        // Décalage regard Y
    float openHeight;   // Hauteur d'ouverture (Clignement)
    MoodState mood;     // Humeur actuelle
};

#endif