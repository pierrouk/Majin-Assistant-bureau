#ifndef PAGE_PERSONALIZATION_H
#define PAGE_PERSONALIZATION_H

#include <Arduino.h>
#include "WebTheme.h"

class PagePersonalization {
public:
    static String getHTML(bool showTime, bool showSensors, String lat, String lon, bool weatherEnabled, String bootText, int manualMood) {
        String html = R"rawliteral(<!DOCTYPE html><html lang="fr"><head><meta charset="UTF-8"><meta name="viewport" content="width=device-width, initial-scale=1.0"><title>Personnalisation</title>)rawliteral";
        html += WebTheme::getCommonCSS();
        html += R"rawliteral(</head><body>)rawliteral";
        html += WebTheme::getNavBar("perso");
        
        html += R"rawliteral(
        <div class="container">
            <h1>Personnalisation</h1>
            
            <!-- APPARENCE -->
            <div class="card">
                <h3>🎨 Apparence</h3>
                
                <label>Expression Visage (Test)</label>
                <select id="mood_select" onchange="saveMood(this.value)">
                    <option value="-1" )rawliteral" + String(manualMood == -1 ? "selected" : "") + R"rawliteral(>🤖 Automatique (Selon Tamagotchi)</option>
                    <option value="0" )rawliteral" + String(manualMood == 0 ? "selected" : "") + R"rawliteral(>😐 Neutre</option>
                    <option value="1" )rawliteral" + String(manualMood == 1 ? "selected" : "") + R"rawliteral(>😊 Heureux</option>
                    <option value="2" )rawliteral" + String(manualMood == 2 ? "selected" : "") + R"rawliteral(>😠 Colère</option>
                    <option value="3" )rawliteral" + String(manualMood == 3 ? "selected" : "") + R"rawliteral(>💤 Sommeil</option>
                    <option value="4" )rawliteral" + String(manualMood == 4 ? "selected" : "") + R"rawliteral(>😓 Fatigué</option>
                    <option value="5" )rawliteral" + String(manualMood == 5 ? "selected" : "") + R"rawliteral(>🍔 Affamé</option>
                    <option value="6" )rawliteral" + String(manualMood == 6 ? "selected" : "") + R"rawliteral(>😒 Ennuyé</option>
                </select>
                <p style="font-size:0.8rem; color:#666;">Forcez une expression pour voir à quoi elle ressemble.</p>

                <hr style="border:0; border-top:1px solid #333; margin:15px 0;">

                <label>Logo de Démarrage (Max 12 car.)</label>
                <div style="display:flex; gap:10px;">
                    <input type="text" id="boot_txt" value=")rawliteral" + bootText + R"rawliteral(" maxlength="12">
                    <button class="btn-small" onclick="saveBoot()">OK</button>
                </div>
            </div>

            <!-- AFFICHAGE -->
            <div class="card">
                <h3>Informations à l'écran</h3>
                <div class="module-row">
                    <div class="module-info"><h3>🕒 Horloge</h3></div>
                    <div class="module-controls"><label class="switch"><input type="checkbox" onchange="toggleDisplay('time', this.checked)" )rawliteral";
        if (showTime) html += "checked";
        html += R"rawliteral(><span class="slider"></span></label></div>
                </div>
                <div class="module-row">
                    <div class="module-info"><h3>🌡️ Capteurs</h3></div>
                    <div class="module-controls"><label class="switch"><input type="checkbox" onchange="toggleDisplay('sensors', this.checked)" )rawliteral";
        if (showSensors) html += "checked";
        html += R"rawliteral(><span class="slider"></span></label></div>
                </div>
            </div>

            <!-- MÉTÉO -->
            <div class="card">
                <h3>🌦️ Météo (Open-Meteo)</h3>
                <div class="module-row">
                    <div class="module-info"><h3>Activer Météo</h3></div>
                    <div class="module-controls"><label class="switch"><input type="checkbox" id="w_enable" )rawliteral";
        if(weatherEnabled) html += "checked";
        html += R"rawliteral(><span class="slider"></span></label></div>
                </div>
                <label>Latitude</label><input type="text" id="w_lat" value=")rawliteral" + lat + R"rawliteral(" placeholder="Ex: 48.8566">
                <label>Longitude</label><input type="text" id="w_lon" value=")rawliteral" + lon + R"rawliteral(" placeholder="Ex: 2.3522">
                <button onclick="saveWeather()">Enregistrer Météo</button>
            </div>
        </div>
        )rawliteral";
        html += WebTheme::getFooter();
        
        html += R"rawliteral(
        <script>
            function toggleDisplay(id, state) { fetch('/api/config/display?id=' + id + '&state=' + (state ? '1' : '0'), {method: 'POST'}); }
            function saveBoot() { fetch('/api/config/boot?text=' + encodeURIComponent(document.getElementById('boot_txt').value), {method:'POST'}).then(()=>{alert('Logo Sauvegardé !');}); }
            function saveWeather() { 
                var p = '?lat='+document.getElementById('w_lat').value+'&lon='+document.getElementById('w_lon').value+'&enable='+(document.getElementById('w_enable').checked?'1':'0');
                fetch('/api/config/weather'+p, {method:'POST'}).then(()=>{alert('Météo Sauvée !');});
            }
            function saveMood(val) {
                fetch('/api/config/mood?val=' + val, {method:'POST'}).then(()=>{console.log('Mood forced to '+val);});
            }
        </script>
        </body></html>)rawliteral";
        return html;
    }
};

#endif