#ifndef VERSION_H
#define VERSION_H

#define MAJIN_VERSION_MAJOR 1
#define MAJIN_VERSION_MINOR 0
#define MAJIN_VERSION_PATCH 0

#define MAJIN_VERSION_NAME "Cyber-Awakening"
#define MAJIN_BUILD_DATE __DATE__ " " __TIME__

// Astuce C++ pour convertir des nombres en string dans un #define
#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)

// Résultat : "v1.0.0" (C'est maintenant une chaîne littérale propre)
#define MAJIN_FW_VERSION "v" STR(MAJIN_VERSION_MAJOR) "." STR(MAJIN_VERSION_MINOR) "." STR(MAJIN_VERSION_PATCH)

#endif