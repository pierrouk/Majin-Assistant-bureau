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
            .grid-btn { aspect-ratio: 1; background: #333; border-radius: 8px; display: flex; flex-direction: column; align-items: center; justify-content: center; cursor: pointer; border: 2px solid #444; transition: 0.2s; position: relative; }
            .grid-btn:hover { border-color: var(--primary); }
            .grid-btn.active { background: #444; border-color: var(--success); }
            .grid-label { font-weight: bold; font-size: 0.9rem; color: white; margin-top: 5px; }
            .grid-icon { font-size: 1.5rem; } /* On utilisera des emojis pour la preview Web */
            
            .pagination { display: flex; justify-content: center; gap: 10px; margin-bottom: 20px; }
            .page-btn { padding: 8px 15px; background: #222; border: 1px solid #444; color: #888; cursor: pointer; border-radius: 4px; text-decoration:none; display:inline-block;}
            .page-btn.active { background: var(--primary); color: black; border-color: var(--primary); font-weight: bold; }

            #edit-form { display: none; background: #222; padding: 15px; border-radius: 8px; border: 1px solid var(--primary); }
            .color-picker { display: flex; gap: 10px; margin: 10px 0; }
            .color-opt { width: 30px; height: 30px; border-radius: 50%; cursor: pointer; border: 2px solid transparent; }
            .mod-group { display: flex; gap: 10px; margin: 10px 0; }
            .mod-check { background: #333; padding: 8px; border-radius: 4px; cursor: pointer; user-select: none; }
            .mod-check input { display: none; }
            .mod-check input:checked + span { color: var(--primary); font-weight: bold; }
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

        // Map Emojis pour preview Web
        // 0:Txt, 1:Play, 2:Mute, 3:Cam, 4:Mic, 5:Obs, 6:File, 7:Save, 8:Home, 9:Chat
        String icons[] = {"T", "▶️", "🔇", "📷", "🎤", "🔴", "📄", "💾", "🏠", "💬"};

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
            String iconSymbol = (btn.iconID < 10) ? icons[btn.iconID] : "T";
            if (!btn.active) { label = "+"; iconSymbol = ""; }
            
            html += "<div class='grid-btn' onclick='editBtn(" + String(globalIdx) + ")' style='border-color:" + colorHex + "'>";
            if(btn.active && btn.iconID > 0) html += "<span class='grid-icon'>" + iconSymbol + "</span>";
            html += "<span class='grid-label' style='color:" + colorHex + "'>" + label + "</span>";
            html += "</div>";
        }

        html += R"rawliteral(
            </div>

            <div id="edit-form">
                <h3 style="margin-top:0">Éditer Bouton #<span id="edit-id-disp"></span></h3>
                <input type="hidden" id="edit-id">
                
                <label>Label</label>
                <input type="text" id="edit-label" maxlength="8">
                
                <label>Icône</label>
                <select id="edit-icon">
                    <option value="0">Aucune (Texte seul)</option>
                    <option value="1">Play ▶️</option>
                    <option value="2">Mute 🔇</option>
                    <option value="3">Caméra 📷</option>
                    <option value="4">Micro 🎤</option>
                    <option value="5">Rec/OBS 🔴</option>
                    <option value="6">Fichier 📄</option>
                    <option value="7">Save 💾</option>
                    <option value="8">Home 🏠</option>
                    <option value="9">Chat 💬</option>
                </select>

                <label>Raccourci</label>
                <div style="display:flex; gap:10px;">
                    <input type="text" id="edit-key" maxlength="1" placeholder="Touche" style="width: 100px;">
                    <div class="mod-group">
                        <label class="mod-check"><input type="checkbox" id="mod-ctrl"><span>CTRL</span></label>
                        <label class="mod-check"><input type="checkbox" id="mod-shift"><span>SHIFT</span></label>
                        <label class="mod-check"><input type="checkbox" id="mod-alt"><span>ALT</span></label>
                    </div>
                </div>

                <label>Couleur</label>
                <div class="color-picker">
                    <div class="color-opt" style="background:#00F0FF" onclick="setColor(2015)"></div>
                    <div class="color-opt" style="background:#FF0055" onclick="setColor(63562)"></div>
                    <div class="color-opt" style="background:#00FF99" onclick="setColor(2035)"></div>
                    <div class="color-opt" style="background:#FF3333" onclick="setColor(63878)"></div>
                    <div class="color-opt" style="background:#FFA500" onclick="setColor(65504)"></div>
                </div>
                <input type="hidden" id="edit-color">

                <div style="display:flex; gap:10px; margin-top:15px;">
                    <button onclick="saveBtn()">Enregistrer</button>
                    <button class="btn-warning" onclick="closeEdit()">Annuler</button>
                </div>
                <button class="btn-danger" onclick="clearBtn()" style="margin-top:10px;">Effacer</button>
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
                window.scrollTo(0, document.body.scrollHeight);
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
                var url = `/api/deck/set?id=${id}&label=${label}&icon=${icon}&key=${key}&color=${color}&ctrl=${ctrl}&shift=${shift}&alt=${alt}`;
                fetch(url, {method: 'POST'}).then(() => location.reload());
            }
            function clearBtn() {
                 var id = document.getElementById('edit-id').value;
                 fetch(`/api/deck/set?id=${id}&active=0`, {method: 'POST'}).then(() => location.reload());
            }
        </script>
        </body></html>)rawliteral";
        return html;
    }
};

#endif