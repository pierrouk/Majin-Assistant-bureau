#ifndef MAJIN_FONT_H
#define MAJIN_FONT_H

#include <LovyanGFX.hpp>

// On utilise une police "Orbitron" ou "Roboto" si disponible, sinon une police système.
// LovyanGFX inclut souvent la famille "FreeSans".
// Pour un look Cyberpunk, "Orbitron" est top si inclus, sinon "FreeSansBold".

// Ici on définit un alias pour que le reste du code l'utilise.
// Vous pouvez changer "&fonts::FreeSansBold12pt7b" par une autre police si vous en ajoutez une custom plus tard.

namespace fonts {
    const auto& MajinFont = fonts::FreeSansBold12pt7b; 
}

#endif