#ifndef SHARED_TYPES_H
#define SHARED_TYPES_H

#include <Arduino.h>

// Scènes de l'interface
enum UIScene { 
    SCENE_BOOT, SCENE_FACE, SCENE_MENU, 
    SCENE_APP_TRACKPAD, SCENE_APP_STREAMDECK, 
    SCENE_SETTINGS, SCENE_SETUP_WIFI, 
    SCENE_COUNTDOWN, SCENE_TAMA_MENU 
};

// Structure d'une Application du menu
struct AppIcon { 
    String name; 
    uint32_t color; 
    int x, y, radius; 
    UIScene targetScene; 
};

// Actions Tamagotchi (C'est ça qui manquait !)
enum TamaAction { 
    ACTION_NONE, 
    ACTION_FEED, 
    ACTION_PLAY, 
    ACTION_SLEEP, 
    ACTION_WAKE 
};

#endif