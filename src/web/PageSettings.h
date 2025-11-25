#ifndef PAGE_SETTINGS_H
#define PAGE_SETTINGS_H

#include <Arduino.h>
#include "WebTheme.h"

class PageSettings {
public:
    static String getHTML() {
        String html = R"rawliteral(<!DOCTYPE html><html lang="fr"><head><meta charset="UTF-8"><meta name="viewport" content="width=device-width, initial-scale=1.0"><title>Paramètres</title>)rawliteral";
        html += WebTheme::getCommonCSS();
        html += R"rawliteral(</head><body>)rawliteral";
        html += WebTheme::getNavBar("settings");
        
        html += R"rawliteral(
        <div class="container">
            <h1>Système</h1>
            
            <div class="card">
                <h3>📶 Réseau WiFi</h3>
                <p>Pour changer de réseau WiFi, le robot doit oublier la configuration actuelle et redémarrer en mode Point d'Accès.</p>
                <button class="btn-warning" onclick="resetWifi()">Oublier le Réseau & Redémarrer</button>
            </div>

            <div class="card" style="border-color: var(--accent);">
                <h3 style="color:var(--accent);">⚠️ Zone de Danger</h3>
                <p>La réinitialisation d'usine effacera toutes les données (Nom, WiFi, Préférences, Tamagotchi).</p>
                <button class="btn-danger" onclick="factoryReset()">RÉINITIALISATION D'USINE</button>
            </div>
        </div>
        )rawliteral";
        html += WebTheme::getFooter();
        
        html += R"rawliteral(
        <script>
            function resetWifi(){if(confirm("Voulez-vous vraiment oublier le réseau WiFi ?")){fetch('/api/wifi/reset',{method:'POST'}).then(()=>{alert("Redémarrage...");});}}
            function factoryReset(){if(confirm("ATTENTION : Cela va tout effacer !")){fetch('/api/reset',{method:'POST'}).then(()=>{alert("Reset...");});}}
        </script>
        </body></html>)rawliteral";
        return html;
    }
};

#endif