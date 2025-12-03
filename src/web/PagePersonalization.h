#ifndef PAGE_PERSONALIZATION_H
#define PAGE_PERSONALIZATION_H

#include <Arduino.h>
#include "WebTheme.h"

class PagePersonalization {
public:
    // On ajoute les arguments de calibration
    static String getHTML(bool showTime, bool showSensors, String lat, String lon, bool weatherEnabled, String city, String bootText, int manualMood, float sleepTh, int scrMin, int scrMax) {
        String html = R"rawliteral(<!DOCTYPE html><html lang="fr"><head><meta charset="UTF-8"><meta name="viewport" content="width=device-width, initial-scale=1.0"><title>Personnalisation</title>)rawliteral";
        html += WebTheme::getCommonCSS();
        html += R"rawliteral(</head><body>)rawliteral";
        html += WebTheme::getNavBar("perso");
        
        html += R"rawliteral(
        <div class="container">
            <h1>Personnalisation</h1>
            
            <div class="card">
                <h3>🎨 Apparence & Humeur</h3>
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
                
                <hr style="border:0; border-top:1px solid #333; margin:15px 0;">

                <label>Logo de Démarrage</label>
                <div style="display:flex; gap:10px;">
                    <input type="text" id="boot_txt" value=")rawliteral" + bootText + R"rawliteral(" maxlength="12">
                    <button class="btn-small" onclick="saveBoot()">OK</button>
                </div>
            </div>

            <div class="card">
                <h3>💡 Calibration Lumière</h3>
                <p style="font-size:0.8rem; color:#888;">Ajustez selon votre boîtier.</p>
                
                <label>Seuil Sommeil Auto (Lux)</label>
                <div style="display:flex; gap:10px; align-items:center;">
                    <input type="range" id="sleep_th" min="0" max="50" step="0.5" value=")rawliteral" + String(sleepTh) + R"rawliteral(" oninput="document.getElementById('val_th').innerText=this.value">
                    <span id="val_th" style="width:40px;">)rawliteral" + String(sleepTh) + R"rawliteral(</span>
                </div>
                <p style="font-size:0.7rem; color:#aaa;">En dessous de cette valeur, Majin dort.</p>

                <hr style="border:0; border-top:1px solid #333; margin:10px 0;">

                <label>Luminosité Écran Min (Nuit)</label>
                <div style="display:flex; gap:10px; align-items:center;">
                    <input type="range" id="scr_min" min="5" max="100" step="5" value=")rawliteral" + String(scrMin) + R"rawliteral(" oninput="document.getElementById('val_min').innerText=this.value">
                    <span id="val_min" style="width:40px;">)rawliteral" + String(scrMin) + R"rawliteral(</span>
                </div>

                <label>Luminosité Écran Max (Jour)</label>
                <div style="display:flex; gap:10px; align-items:center;">
                    <input type="range" id="scr_max" min="100" max="255" step="5" value=")rawliteral" + String(scrMax) + R"rawliteral(" oninput="document.getElementById('val_max').innerText=this.value">
                    <span id="val_max" style="width:40px;">)rawliteral" + String(scrMax) + R"rawliteral(</span>
                </div>
                
                <button onclick="saveLightConfig()" style="margin-top:10px;">Appliquer Calibration</button>
            </div>

            <div class="card">
                <h3>Informations Écran</h3>
                <div class="module-row">
                    <div class="module-info"><h3>🕒 Horloge</h3></div>
                    <div class="module-controls"><label class="switch"><input type="checkbox" onchange="toggleDisplay('time', this.checked)" )rawliteral";
        if (showTime) html += "checked";
        html += R"rawliteral(><span class="slider"></span></label></div>
                </div>
                <div class="module-row">
                    <div class="module-info"><h3>🌡️ Capteurs Internes</h3></div>
                    <div class="module-controls"><label class="switch"><input type="checkbox" onchange="toggleDisplay('sensors', this.checked)" )rawliteral";
        if (showSensors) html += "checked";
        html += R"rawliteral(><span class="slider"></span></label></div>
                </div>
            </div>

            <div class="card">
                <h3>🌦️ Météo Locale</h3>
                <div class="module-row">
                    <div class="module-info"><h3>Activer Widget</h3></div>
                    <div class="module-controls"><label class="switch"><input type="checkbox" id="w_enable" )rawliteral";
        if(weatherEnabled) html += "checked";
        html += R"rawliteral(><span class="slider"></span></label></div>
                </div>
                <label>Nom de la Ville</label>
                <input type="text" id="w_city" value=")rawliteral" + city + R"rawliteral(">
                <div style="display:grid; grid-template-columns: 1fr 1fr; gap:10px; margin-top:10px;">
                    <div><label>Lat</label><input type="text" id="w_lat" value=")rawliteral" + lat + R"rawliteral("></div>
                    <div><label>Lon</label><input type="text" id="w_lon" value=")rawliteral" + lon + R"rawliteral("></div>
                </div>
                <button onclick="saveWeather()" style="margin-top:15px;">Enregistrer Météo</button>
            </div>
        </div>
        )rawliteral";
        html += WebTheme::getFooter();
        
        html += R"rawliteral(
        <script>
            function toggleDisplay(id, state) { fetch('/api/config/display?id=' + id + '&state=' + (state ? '1' : '0'), {method: 'POST'}); }
            function saveBoot() { fetch('/api/config/boot?text=' + encodeURIComponent(document.getElementById('boot_txt').value), {method:'POST'}).then(()=>{alert('Logo Sauvegardé !');}); }
            function saveWeather() { 
                var lat = document.getElementById('w_lat').value;
                var lon = document.getElementById('w_lon').value;
                var city = encodeURIComponent(document.getElementById('w_city').value);
                var enable = document.getElementById('w_enable').checked ? '1' : '0';
                fetch('/api/config/weather?lat=' + lat + '&lon=' + lon + '&city=' + city + '&enable=' + enable, {method:'POST'})
                    .then(() => { alert('Météo Sauvée !'); location.reload(); });
            }
            function saveMood(val) { fetch('/api/config/mood?val=' + val, {method:'POST'}); }
            
            // NOUVEAU : Sauvegarde Calibration
            function saveLightConfig() {
                var th = document.getElementById('sleep_th').value;
                var min = document.getElementById('scr_min').value;
                var max = document.getElementById('scr_max').value;
                fetch('/api/config/light?th=' + th + '&min=' + min + '&max=' + max, {method:'POST'})
                    .then(() => { alert('Calibration Appliquée !'); });
            }
        </script>
        </body></html>)rawliteral";
        return html;
    }
};
#endif