#ifndef MENU_RENDERER_H
#define MENU_RENDERER_H

#include <Arduino.h>
#include <LovyanGFX.hpp>
#include <vector>
#include "../include/SharedTypes.h" // Nos types communs

class MenuRenderer {
public:
    MenuRenderer();
    
    // Configuration
    void addApp(String name, uint32_t color, UIScene target);
    
    // Boucle principale
    void update();
    void draw(LGFX_Sprite* sprite);
    
    // Gestion Tactile
    // Retourne la scène cible si clic valide, sinon SCENE_MENU (reste sur place)
    UIScene handleClick(int touchX, int touchY);
String getSelectedAppName();
private:
    std::vector<AppIcon> _apps;
    
    // Variables de scroll et d'animation
    float _scrollCurrent = 0.0;
    int   _scrollTarget = 0;
    
    // Helper graphique
    void _drawHexagon(LGFX_Sprite* spr, int x, int y, int radius, uint32_t color, bool filled);
};

#endif