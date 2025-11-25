#ifndef PAGE_TAMAGOTCHI_H
#define PAGE_TAMAGOTCHI_H

#include <Arduino.h>
#include "WebTheme.h"

class PageTamagotchi {
public:
    static String getHTML() {
        String html = R"rawliteral(<!DOCTYPE html><html lang="fr"><head><meta charset="UTF-8"><meta name="viewport" content="width=device-width, initial-scale=1.0"><title>Majin Life</title>)rawliteral";
        html += WebTheme::getCommonCSS();
        html += R"rawliteral(</head><body>)rawliteral";
        html += WebTheme::getNavBar("tamagotchi"); // Lien à ajouter dans navbar
        
        html += R"rawliteral(
        <div class="container">
            <h1>État Vital</h1>
            
            <!-- JAUGES -->
            <div class="card">
                <h3>Statistiques</h3>
                
                <label>ÉNERGIE (Sommeil)</label>
                <div style="background:#333; border-radius:10px; height:20px; margin-bottom:15px;">
                    <div id="bar-energy" style="width:0%; height:100%; background:var(--success); border-radius:10px; transition:1s;"></div>
                </div>

                <label>FUN (Activité)</label>
                <div style="background:#333; border-radius:10px; height:20px; margin-bottom:15px;">
                    <div id="bar-fun" style="width:0%; height:100%; background:var(--primary); border-radius:10px; transition:1s;"></div>
                </div>

                <label>SATIÉTÉ (Inverse de Faim)</label>
                <div style="background:#333; border-radius:10px; height:20px; margin-bottom:15px;">
                    <div id="bar-hunger" style="width:0%; height:100%; background:#FFA500; border-radius:10px; transition:1s;"></div>
                </div>
            </div>

            <!-- ACTIONS -->
            <div class="card">
                <h3>Interactions</h3>
                <div style="display:grid; grid-template-columns: 1fr 1fr; gap:10px;">
                    <button class="btn" onclick="action('feed')">🍎 Nourrir</button>
                    <button class="btn" onclick="action('play')">🎮 Jouer</button>
                    <button class="btn" onclick="action('sleep')">💤 Dormir</button>
                    <button class="btn" onclick="action('wake')">☀️ Réveiller</button>
                </div>
            </div>
            
            <!-- ADMIN -->
            <div class="card" style="border-color: var(--accent)">
                <h3 style="color: var(--accent)">Admin / Cheat</h3>
                <button class="btn-danger" onclick="action('reset_stats')">Reset Stats (Full 100%)</button>
            </div>
        </div>
        )rawliteral";
        html += WebTheme::getFooter();
        
        html += R"rawliteral(
        <script>
            function updateStats() {
                fetch('/api/tamagotchi/status').then(r=>r.json()).then(d => {
                    // Energie
                    document.getElementById('bar-energy').style.width = d.energy + "%";
                    // Fun
                    document.getElementById('bar-fun').style.width = d.fun + "%";
                    // Faim (Inversé pour affichage : 0% faim = 100% barre)
                    document.getElementById('bar-hunger').style.width = (100 - d.hunger) + "%";
                });
            }
            
            function action(type) {
                fetch('/api/tamagotchi/action?type=' + type, {method:'POST'}).then(() => {
                    updateStats(); // Update immédiat
                });
            }

            setInterval(updateStats, 2000);
            updateStats();
        </script>
        </body></html>)rawliteral";
        return html;
    }
};

#endif