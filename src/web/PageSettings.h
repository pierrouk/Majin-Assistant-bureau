#ifndef PAGE_SETTINGS_H
#define PAGE_SETTINGS_H

#include <Arduino.h>
#include "WebTheme.h"

class PageSettings {
public:
    static String getHTML() {
        String html = R"rawliteral(<!DOCTYPE html><html lang="fr"><head><meta charset="UTF-8"><meta name="viewport" content="width=device-width, initial-scale=1.0"><title>Paramètres</title>)rawliteral";
        html += WebTheme::getCommonCSS();
        // CSS Spécifique pour la barre de progression
        html += R"rawliteral(
        <style>
            #progress-container { width: 100%; background-color: #333; border-radius: 8px; margin-top: 10px; display: none; overflow: hidden; }
            #progress-bar { width: 0%; height: 25px; background-color: var(--primary); text-align: center; line-height: 25px; color: black; font-weight: bold; transition: width 0.2s; }
        </style>
        </head><body>)rawliteral";
        html += WebTheme::getNavBar("settings");
        
        html += R"rawliteral(
        <div class="container">
            <h1>Système</h1>
            
            <div class="card" style="border-color: var(--primary);">
                <h3>🚀 Mise à jour (OTA)</h3>
                <p>Sélectionnez le fichier <strong>firmware.bin</strong> généré par PlatformIO (dossier .pio/build/...).</p>
                
                <form id="upload_form" method="POST" action="#" enctype="multipart/form-data">
                    <input type="file" name="update" id="file_input" accept=".bin">
                    <button type="button" onclick="startUpload()" style="margin-top:10px;">Lancer la Mise à jour</button>
                </form>

                <div id="progress-container">
                    <div id="progress-bar">0%</div>
                </div>
                <p id="status-msg" style="color: #888; font-size: 0.9rem; margin-top: 5px;"></p>
            </div>

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

            function startUpload() {
                var fileInput = document.getElementById('file_input');
                var file = fileInput.files[0];
                if (!file) { alert("Veuillez choisir un fichier .bin !"); return; }

                var formdata = new FormData();
                formdata.append("update", file);
                
                var ajax = new XMLHttpRequest();
                
                // Barre de progression
                ajax.upload.addEventListener("progress", function(e) {
                    var percent = Math.round((e.loaded / e.total) * 100);
                    document.getElementById('progress-container').style.display = 'block';
                    document.getElementById('progress-bar').style.width = percent + '%';
                    document.getElementById('progress-bar').innerText = percent + '%';
                    document.getElementById('status-msg').innerText = "Téléversement en cours... Ne pas débrancher !";
                }, false);

                // Fin du transfert
                ajax.addEventListener("load", function(e) {
                    document.getElementById('status-msg').innerText = "Installation terminée. Redémarrage...";
                    document.getElementById('progress-bar').style.backgroundColor = "var(--success)";
                    setTimeout(function(){ location.reload(); }, 5000); // Reload après 5s
                }, false);

                ajax.addEventListener("error", function(e) {
                    alert("Erreur lors du transfert !");
                    document.getElementById('status-msg').innerText = "Erreur.";
                }, false);

                ajax.open("POST", "/update");
                ajax.send(formdata);
            }
        </script>
        </body></html>)rawliteral";
        return html;
    }
};

#endif