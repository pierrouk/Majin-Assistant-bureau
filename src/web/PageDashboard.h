#ifndef PAGE_DASHBOARD_H
#define PAGE_DASHBOARD_H

#include <Arduino.h>
#include "WebTheme.h"

class PageDashboard {
public:
    static String getHTML() {
        String html = R"rawliteral(<!DOCTYPE html><html lang="fr"><head><meta charset="UTF-8"><meta name="viewport" content="width=device-width, initial-scale=1.0"><title>Majin Dashboard</title>)rawliteral";
        html += WebTheme::getCommonCSS();
        html += R"rawliteral(</head><body>)rawliteral";
        html += WebTheme::getNavBar("home");
        html += R"rawliteral(
        <div class="container">
            <div class="card">
                <h1 id="r-name">...</h1>
                <h2 id="r-ip">...</h2>
                <p>Bienvenue sur le tableau de bord principal. Je suis en ligne et fonctionnel.</p>
            </div>
            
            <div class="card">
                <h3 style="margin-top:0; color:#aaa;">TÉLÉMÉTRIE</h3>
                <div class="data-grid">
                    <div class="data-box"><span class="data-label">TEMPÉRATURE</span><div class="data-value cyan" id="val-temp">--°C</div></div>
                    <div class="data-box"><span class="data-label">HUMIDITÉ</span><div class="data-value" id="val-hum">--%</div></div>
                    <div class="data-box"><span class="data-label">LUMIÈRE</span><div class="data-value" id="val-lux">-- Lx</div></div>
                    <div class="data-box"><span class="data-label">ÉNERGIE</span><div class="data-value green" id="val-nrg">--%</div></div>
                    <div class="data-box"><span class="data-label">WIFI</span><div class="data-value rose" id="val-rssi">-- dBm</div></div>
                </div>
            </div>
            
            <div class="card">
                <h3 style="margin-top:0; color:#aaa;">ACTIONS RAPIDES</h3>
                <div style="display:flex; gap:10px;">
                    <button onclick="fetch('/api/hello')">👋 Bonjour</button>
                    <button style="background:var(--accent); color:white;" onclick="fetch('/api/dance')">💃 Danse</button>
                </div>
            </div>
        </div>
        )rawliteral";
        html += WebTheme::getFooter();
        
        // SCRIPT PLUS ROBUSTE
        html += R"rawliteral(
        <script>
            function updateData() {
                fetch('/api/status')
                    .then(response => {
                        if (!response.ok) throw new Error('Network response was not ok');
                        return response.json();
                    })
                    .then(data => {
                        // On vérifie si l'élément existe avant de le mettre à jour pour éviter les erreurs console
                        if(document.getElementById('r-name')) document.getElementById('r-name').innerText = data.name;
                        if(document.getElementById('r-ip')) document.getElementById('r-ip').innerText = data.ip;
                        
                        if(document.getElementById('val-temp')) document.getElementById('val-temp').innerText = data.temp + "°C";
                        if(document.getElementById('val-hum')) document.getElementById('val-hum').innerText = data.hum + "%";
                        if(document.getElementById('val-lux')) document.getElementById('val-lux').innerText = data.lux + " Lx";
                        if(document.getElementById('val-nrg')) document.getElementById('val-nrg').innerText = data.energy + "%";
                        if(document.getElementById('val-rssi')) document.getElementById('val-rssi').innerText = data.rssi + " dBm";
                    })
                    .catch(error => {
                        console.error('Erreur fetch status:', error);
                    });
            }
            
            // Premier appel
            updateData();
            // Rafraichissement toutes les 2 secondes
            setInterval(updateData, 2000);
        </script>
        </body></html>)rawliteral";
        return html;
    }
};

#endif