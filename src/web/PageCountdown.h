#ifndef PAGE_COUNTDOWN_H
#define PAGE_COUNTDOWN_H

#include <Arduino.h>
#include "WebTheme.h"

class PageCountdown {
public:
    // Ajout argument 'holidays'
    static String getHTML(String name, unsigned long timestamp, int type, int holidays) {
        String html = R"rawliteral(<!DOCTYPE html><html lang="fr"><head><meta charset="UTF-8"><meta name="viewport" content="width=device-width, initial-scale=1.0"><title>Compte à Rebours</title>)rawliteral";
        html += WebTheme::getCommonCSS();
        html += R"rawliteral(</head><body>)rawliteral";
        html += WebTheme::getNavBar("countdown");
        
        html += R"rawliteral(
        <div class="container">
            <h1>Gestion Événement</h1>
            
            )rawliteral";
            
            if (timestamp > 0 && name.length() > 0) {
                html += R"rawliteral(
                <div class="card" style="border: 1px solid var(--success); background: rgba(0, 255, 153, 0.05);">
                    <h3 style="color:var(--success); margin-top:0;">✅ Événement Actif</h3>
                    <h1 style="font-size: 2.5rem; margin: 10px 0; color: white;">)rawliteral" + name + R"rawliteral(</h1>
                    <p id="display-date" style="font-size: 1.2rem; color: #ccc;">...</p>
                    <p style="font-size: 0.9rem; color: #888;">Congés déduits : )rawliteral" + String(holidays) + R"rawliteral( jours</p>
                </div>
                )rawliteral";
            }

            html += R"rawliteral(

            <div class="card">
                <h3>🎯 Configurer</h3>
                <form action="/api/countdown/set" method="POST" onsubmit="return prepareDate()">
                    <label>Nom de l'événement</label>
                    <input type="text" id="evt_name" name="name" value=")rawliteral" + name + R"rawliteral(" placeholder="Ex: Vacances..." required>
                    
                    <label>Date Cible</label>
                    <input type="datetime-local" id="evt_date" required>
                    <input type="hidden" id="evt_timestamp" name="timestamp">
                    
                    <div style="margin: 15px 0;">
                        <label>Jours de Congés (à déduire)</label>
                        <input type="number" name="holidays" value=")rawliteral" + String(holidays) + R"rawliteral(" min="0" max="100" placeholder="0">
                        <p style="font-size:0.8rem; color:#666;">Sera soustrait du calcul des jours ouvrés.</p>
                    </div>

                    <label>Type d'animation</label>
                    <select name="type" id="evt_type">
                        <option value="0" )rawliteral" + String(type==0?"selected":"") + R"rawliteral(>Standard (Bleu)</option>
                        <option value="1" )rawliteral" + String(type==1?"selected":"") + R"rawliteral(>Anniversaire (Fête)</option>
                        <option value="2" )rawliteral" + String(type==2?"selected":"") + R"rawliteral(>Noël (Neige)</option>
                        <option value="3" )rawliteral" + String(type==3?"selected":"") + R"rawliteral(>Vacances (Soleil)</option>
                        <option value="4" )rawliteral" + String(type==4?"selected":"") + R"rawliteral(>Mariage (Bague)</option>
                        <option value="5" )rawliteral" + String(type==5?"selected":"") + R"rawliteral(>Montagne (Paysage)</option>
                    </select>
                    
                    <button type="submit">Enregistrer</button>
                </form>
                
                <br>
                <button class="btn-danger" onclick="clearEvent()">Supprimer l'événement</button>
            </div>
        </div>
        )rawliteral";
        html += WebTheme::getFooter();
        
        html += R"rawliteral(
        <script>
            var serverTimestamp = )rawliteral" + String(timestamp) + R"rawliteral(;
            if(serverTimestamp > 0) {
                var date = new Date(serverTimestamp * 1000);
                var localDate = new Date(date.getTime() - (date.getTimezoneOffset() * 60000));
                document.getElementById('evt_date').value = localDate.toISOString().slice(0,16);
                
                var disp = document.getElementById('display-date');
                if(disp) {
                    var options = { weekday: 'long', year: 'numeric', month: 'long', day: 'numeric', hour: '2-digit', minute: '2-digit' };
                    disp.innerText = date.toLocaleDateString('fr-FR', options);
                }
            }

            function prepareDate() {
                var input = document.getElementById('evt_date').value;
                if(!input) return false;
                var date = new Date(input);
                document.getElementById('evt_timestamp').value = Math.floor(date.getTime() / 1000);
                return true;
            }
            
            function clearEvent() {
                if(confirm("Supprimer ?")) {
                    fetch('/api/countdown/delete', {method:'POST'}).then(() => window.location.reload());
                }
            }
        </script>
        </body></html>)rawliteral";
        return html;
    }
};
#endif