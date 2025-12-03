#ifndef PAGE_DECK_CONFIG_H
#define PAGE_DECK_CONFIG_H

#include <Arduino.h>
#include "WebTheme.h"
#include "../managers/SettingsManager.h" 

class PageDeckConfig {
public:
    static String getHTML(SettingsManager* settings, int pageIndex) {
        String html = R"rawliteral(<!DOCTYPE html><html lang="fr"><head><meta charset="UTF-8"><meta name="viewport" content="width=device-width, initial-scale=1.0"><title>Config Deck</title>)rawliteral";
        html += WebTheme::getCommonCSS();
        html += R"rawliteral(
        <style>
            .grid-editor { display: grid; grid-template-columns: repeat(3, 1fr); gap: 10px; margin-bottom: 20px; }
            .grid-btn { aspect-ratio: 1; background: #333; border-radius: 12px; display: flex; flex-direction: column; align-items: center; justify-content: center; cursor: pointer; border: 2px solid #444; transition: 0.2s; position: relative; box-shadow: 0 4px 6px rgba(0,0,0,0.3); }
            .grid-btn:hover { border-color: var(--primary); transform: translateY(-2px); }
            .grid-btn.active { background: #444; border-color: var(--success); }
            .grid-label { font-weight: bold; font-size: 0.8rem; color: white; margin-top: 5px; text-align:center; overflow:hidden; width:90%; white-space:nowrap; text-overflow:ellipsis; }
            .grid-icon { font-size: 1.8rem; } 
            
            .pagination { display: flex; justify-content: center; gap: 10px; margin-bottom: 20px; }
            .page-btn { padding: 8px 15px; background: #222; border: 1px solid #444; color: #888; cursor: pointer; border-radius: 4px; text-decoration:none; display:inline-block;}
            .page-btn.active { background: var(--primary); color: black; border-color: var(--primary); font-weight: bold; }

            #edit-form { display: none; background: #222; padding: 15px; border-radius: 8px; border: 1px solid var(--primary); box-shadow: 0 0 20px rgba(0,0,0,0.5); }
            .color-picker { display: flex; gap: 10px; margin: 10px 0; flex-wrap: wrap; }
            .color-opt { width: 35px; height: 35px; border-radius: 50%; cursor: pointer; border: 2px solid transparent; transition:0.2s; }
            .color-opt:hover { transform:scale(1.1); border-color:white; }
            
            .mod-group { display: flex; gap: 10px; margin: 10px 0; }
            .mod-check { background: #333; padding: 8px; border-radius: 4px; cursor: pointer; user-select: none; flex:1; text-align:center; border:1px solid #444; }
            .mod-check input { display: none; }
            .mod-check input:checked + span { color: var(--primary); font-weight: bold; text-shadow: 0 0 5px var(--primary); }
        </style>
        </head><body>)rawliteral";
        
        html += WebTheme::getNavBar("deck");
        
        html += R"rawliteral(
        <div class="container">
            <h1>Majin Deck</h1>
            
            <div class="pagination">
        )rawliteral";
        
        for(int p=0; p<DECK_PAGES; p++) {
            html += "<a href='/deck?page=" + String(p) + "' class='page-btn " + String(p==pageIndex?"active":"") + "'>Page " + String(p+1) + "</a>";
        }

        html += R"rawliteral(
            </div>
            <p style="text-align:center; color:#888;">Page )rawliteral" + String(pageIndex+1) + R"rawliteral(</p>
            
            <div class="grid-editor">
        )rawliteral";

        String icons[] = {
            "T", "▶️", "🔀", "📷", "🎤", "🔴", "📄", "💾", "🏠", "💬", 
            "🎮", "🎥", "🌐", "📁", "🔇", "🚫" 
        };

        for (int i = 0; i < 6; i++) {
            int globalIdx = (pageIndex * 6) + i;
            DeckButton btn = settings->getDeckButton(globalIdx);
            
            String colorHex = "#444444"; 
            if (btn.color == 0x079F) colorHex = "#00F0FF"; 
            else if (btn.color == 0xF80A) colorHex = "#FF0055"; 
            else if (btn.color == 0x07F3) colorHex = "#00FF99"; 
            else if (btn.color == 0xF986) colorHex = "#FF3333"; 
            else if (btn.color == 0xFFE0) colorHex = "#FFA500"; 

            String label = String(btn.label);
            int iconId = btn.iconID;
            if (iconId > 15) iconId = 0;
            String iconSymbol = (iconId > 0) ? icons[iconId] : "T";
            
            if (!btn.active) { label = "Vide"; iconSymbol = "+"; }
            
            html += "<div class='grid-btn' onclick='editBtn(" + String(globalIdx) + ")' style='border-color:" + colorHex + "'>";
            if(btn.active && btn.iconID > 0) html += "<span class='grid-icon' style='color:" + colorHex + "'>" + iconSymbol + "</span>";
            else if (!btn.active) html += "<span class='grid-icon' style='color:#666'>+</span>";
            
            // ⚠️ CORRECTION ICI : Ajout de String() autour de la condition ternaire
            html += "<span class='grid-label' style='color:" + String(btn.active ? "white" : "#666") + "'>" + label + "</span>";
            html += "</div>";
        }

        html += R"rawliteral(
            </div>

            <div id="edit-form">
                <h3 style="margin-top:0; color:var(--primary);">Éditer Bouton #<span id="edit-id-disp"></span></h3>
                <input type="hidden" id="edit-id">
                
                <label>Nom du Bouton</label>
                <input type="text" id="edit-label" maxlength="9" placeholder="Ex: OBS Scene">
                
                <label>Icône</label>
                <select id="edit-icon">
                    <option value="0">Aucune (Texte seul)</option>
                    <optgroup label="Contrôle">
                        <option value="1">Play ▶️</option>
                        <option value="2">Shuffle 🔀</option>
                        <option value="5">Rec 🔴</option>
                        <option value="7">Save 💾</option>
                        <option value="8">Home 🏠</option>
                    </optgroup>
                    <optgroup label="Streaming & Social">
                        <option value="3">Caméra On 📷</option>
                        <option value="15">Caméra Off 🚫</option>
                        <option value="4">Micro On 🎤</option>
                        <option value="14">Micro Mute 🔇</option>
                        <option value="11">OBS Studio 🎥</option>
                        <option value="10">Discord 🎮</option>
                        <option value="9">Chat 💬</option>
                    </optgroup>
                    <optgroup label="Système">
                        <option value="12">Chrome / Web 🌐</option>
                        <option value="13">Dossier 📁</option>
                        <option value="6">Fichier 📄</option>
                    </optgroup>
                </select>

                <label>Raccourci Clavier</label>
                <div style="display:flex; gap:10px;">
                    <div class="mod-group" style="flex:2;">
                        <label class="mod-check"><input type="checkbox" id="mod-ctrl"><span>CTRL</span></label>
                        <label class="mod-check"><input type="checkbox" id="mod-shift"><span>SHIFT</span></label>
                        <label class="mod-check"><input type="checkbox" id="mod-alt"><span>ALT</span></label>
                    </div>
                    <input type="text" id="edit-key" maxlength="1" placeholder="Touche" style="flex:1; text-align:center; font-weight:bold; text-transform:uppercase;">
                </div>

                <label>Couleur Néon</label>
                <div class="color-picker">
                    <div class="color-opt" style="background:#00F0FF; box-shadow:0 0 10px #00F0FF;" onclick="setColor(2015)"></div>
                    <div class="color-opt" style="background:#FF0055; box-shadow:0 0 10px #FF0055;" onclick="setColor(63562)"></div>
                    <div class="color-opt" style="background:#00FF99; box-shadow:0 0 10px #00FF99;" onclick="setColor(2035)"></div>
                    <div class="color-opt" style="background:#FF3333; box-shadow:0 0 10px #FF3333;" onclick="setColor(63878)"></div>
                    <div class="color-opt" style="background:#FFA500; box-shadow:0 0 10px #FFA500;" onclick="setColor(65504)"></div>
                </div>
                <input type="hidden" id="edit-color">

                <div style="display:flex; gap:10px; margin-top:20px;">
                    <button onclick="saveBtn()" style="flex:2;">Sauvegarder</button>
                    <button class="btn-warning" onclick="closeEdit()" style="flex:1;">Annuler</button>
                </div>
                <button class="btn-danger" onclick="clearBtn()" style="margin-top:10px; width:100%;">Effacer le bouton</button>
            </div>
        </div>
        )rawliteral";
        
        html += WebTheme::getFooter();
        
        html += R"rawliteral(
        <script>
            function editBtn(id) {
                document.getElementById('edit-form').style.display = 'block';
                document.getElementById('edit-id').value = id;
                document.getElementById('edit-id-disp').innerText = id + 1;
                document.getElementById('edit-label').value = "";
                document.getElementById('edit-key').value = "";
                document.getElementById('edit-icon').value = "0";
                ['mod-ctrl','mod-shift','mod-alt'].forEach(id => document.getElementById(id).checked = false);
                document.getElementById('edit-form').scrollIntoView({behavior: "smooth"});
            }
            function setColor(val) { document.getElementById('edit-color').value = val; }
            function closeEdit() { document.getElementById('edit-form').style.display = 'none'; }
            function saveBtn() {
                var id = document.getElementById('edit-id').value;
                var label = document.getElementById('edit-label').value;
                var icon = document.getElementById('edit-icon').value;
                var key = document.getElementById('edit-key').value;
                var color = document.getElementById('edit-color').value || "2015";
                var ctrl = document.getElementById('mod-ctrl').checked ? '1' : '0';
                var shift = document.getElementById('mod-shift').checked ? '1' : '0';
                var alt = document.getElementById('mod-alt').checked ? '1' : '0';
                if(!label && !key) { alert("Il faut au moins un nom ou une touche !"); return; }
                var url = `/api/deck/set?id=${id}&label=${encodeURIComponent(label)}&icon=${icon}&key=${encodeURIComponent(key)}&color=${color}&ctrl=${ctrl}&shift=${shift}&alt=${alt}`;
                fetch(url, {method: 'POST'}).then(r => { if(r.ok) location.reload(); else alert("Erreur sauvegarde"); });
            }
            function clearBtn() {
                 if(confirm("Effacer ce bouton ?")) {
                     var id = document.getElementById('edit-id').value;
                     fetch(`/api/deck/set?id=${id}&active=0`, {method: 'POST'}).then(() => location.reload());
                 }
            }
        </script>
        </body></html>)rawliteral";
        return html;
    }
};

#endif