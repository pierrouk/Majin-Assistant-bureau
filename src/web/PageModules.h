#ifndef PAGE_MODULES_H
#define PAGE_MODULES_H

#include <Arduino.h>
#include "WebTheme.h"

class PageModules {
public:
    static String getHTML(bool buzzerEnabled, bool servoEnabled) {
        String html = R"rawliteral(<!DOCTYPE html><html lang="fr"><head><meta charset="UTF-8"><meta name="viewport" content="width=device-width, initial-scale=1.0"><title>Modules - Majin OS</title>)rawliteral";
        html += WebTheme::getCommonCSS();
        html += R"rawliteral(</head><body>)rawliteral";
        html += WebTheme::getNavBar("modules");
        html += R"rawliteral(
        <div class="container">
            <h1>Gestion Matériel</h1>
            
            <!-- BUZZER -->
            <div class="card">
                <div class="module-row">
                    <div class="module-info">
                        <h3>🔊 Système Audio (Buzzer)</h3>
                        <p>Contrôle les bips, musiques et notifications sonores.</p>
                    </div>
                    <div class="module-controls">
                        <button class="btn btn-small" onclick="testModule('buzzer')">TEST</button>
                        <label class="switch">
                            <input type="checkbox" id="buzzer-sw" onchange="toggleModule('buzzer', this.checked)" )rawliteral";
        if (buzzerEnabled) html += "checked";
        html += R"rawliteral(>
                            <span class="slider"></span>
                        </label>
                    </div>
                </div>
            </div>

            <!-- SERVO -->
            <div class="card">
                <div class="module-row">
                    <div class="module-info">
                        <h3>⚙️ Moteur Tête (Servo)</h3>
                        <p>Contrôle les mouvements de tête et les danses.</p>
                    </div>
                    <div class="module-controls">
                        <button class="btn btn-small" onclick="testModule('servo')">TEST</button>
                        <label class="switch">
                            <input type="checkbox" id="servo-sw" onchange="toggleModule('servo', this.checked)" )rawliteral";
        if (servoEnabled) html += "checked";
        html += R"rawliteral(>
                            <span class="slider"></span>
                        </label>
                    </div>
                </div>
            </div>
        </div>
        )rawliteral";
        html += WebTheme::getFooter();
        html += R"rawliteral(
        <script>
            function toggleModule(id, state) {
                fetch('/api/module/set?id=' + id + '&state=' + (state ? '1' : '0'), {method: 'POST'})
                    .then(r => console.log(id + " set to " + state));
            }
            function testModule(id) { fetch('/api/module/test?id=' + id, {method: 'POST'}); }
        </script>
        </body></html>)rawliteral";
        return html;
    }
};

#endif