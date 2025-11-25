#ifndef WEB_THEME_H
#define WEB_THEME_H
#include <Arduino.h>

class WebTheme {
public:
    static String getCommonCSS() {
        // ... (CSS identique à la version précédente, je l'abrége ici pour le respect des quotas) ...
        // ... Assume que le CSS est le même que dans le message précédent ...
        return R"rawliteral(
<style>
  :root { --bg-color: #0d0d0d; --card-bg: #1a1a1a; --text-main: #e0e0e0; --primary: #00F0FF; --primary-dim: #008f99; --accent: #FF0055; --success: #00FF99; --nav-bg: #000000; }
  body { font-family: 'Segoe UI', Roboto, Helvetica, sans-serif; background-color: var(--bg-color); color: var(--text-main); margin: 0; padding: 0; padding-bottom: 40px; }
  .navbar { background: var(--nav-bg); border-bottom: 2px solid var(--primary-dim); padding: 10px 20px; display: flex; justify-content: space-between; align-items: center; position: sticky; top: 0; z-index: 100; box-shadow: 0 4px 10px rgba(0,240,255,0.1); }
  .nav-brand { font-weight: bold; font-size: 1.2rem; color: var(--primary); text-decoration: none; display: flex; align-items: center; gap: 10px;}
  .nav-links { display: flex; flex-wrap: wrap; gap: 10px; justify-content: flex-end; }
  .nav-links a { color: #aaa; text-decoration: none; margin-left: 5px; font-size: 0.9rem; transition: 0.2s; white-space: nowrap; }
  .nav-links a:hover, .nav-links a.active { color: #fff; text-shadow: 0 0 5px var(--primary); border-bottom: 1px solid var(--primary); }
  .container { max-width: 800px; margin: 20px auto; padding: 0 15px; }
  .card { background: var(--card-bg); border-radius: 12px; padding: 20px; margin-bottom: 20px; border: 1px solid #333; box-shadow: 0 4px 15px rgba(0,0,0,0.3); }
  .data-grid { display: grid; grid-template-columns: repeat(auto-fit, minmax(100px, 1fr)); gap: 10px; margin-top: 15px; }
  .data-box { background: #111; padding: 10px; border-radius: 8px; text-align: center; border: 1px solid #2a2a2a; }
  .data-label { font-size: 0.8rem; color: #666; display: block; margin-bottom: 5px; }
  .data-value { font-size: 1.2rem; font-weight: bold; color: #fff; }
  .data-value.cyan { color: var(--primary); } .data-value.green { color: var(--success); } .data-value.rose { color: var(--accent); }
  .module-row { display: flex; align-items: center; justify-content: space-between; padding: 15px 0; border-bottom: 1px solid #333; }
  .module-info h3 { margin: 0; color: #fff; } .module-info p { margin: 5px 0 0; color: #888; font-size: 0.9rem; }
  .module-controls { display: flex; align-items: center; gap: 15px; }
  .switch { position: relative; display: inline-block; width: 60px; height: 30px; } .switch input { opacity: 0; width: 0; height: 0; }
  .slider { position: absolute; cursor: pointer; top: 0; left: 0; right: 0; bottom: 0; background-color: #333; transition: .4s; border-radius: 30px; border: 1px solid #555; }
  .slider:before { position: absolute; content: ""; height: 22px; width: 22px; left: 4px; bottom: 3px; background-color: white; transition: .4s; border-radius: 50%; }
  input:checked + .slider { background-color: var(--primary); border-color: var(--primary); box-shadow: 0 0 10px var(--primary-dim); } input:checked + .slider:before { transform: translateX(30px); }
  button, .btn { display: block; width: 100%; padding: 12px; margin: 10px 0; border: none; border-radius: 6px; cursor: pointer; background: var(--primary); color: #000; font-weight: bold; font-size: 1rem; transition: 0.2s; }
  button:hover { transform: translateY(-2px); box-shadow: 0 0 15px var(--primary-dim); background: #fff; }
  .btn-danger { background: var(--accent); color: white; } .btn-warning { background: #FFA500; color: black; } .btn-small { padding: 8px 15px; font-size: 0.8rem; width: auto; display: inline-block; margin: 0; }
  .footer { text-align: center; font-size: 0.8rem; color: #444; margin-top: 30px; }
  .welcome-box { background: rgba(0, 240, 255, 0.05); border-left: 3px solid var(--primary); padding: 15px; text-align: left; }
  input[type="text"], input[type="datetime-local"], select { width: 100%; padding: 10px; background: #222; border: 1px solid #444; color: white; border-radius: 4px; box-sizing: border-box; }
</style>
        )rawliteral";
    }

    // ⚠️ MISE À JOUR DU MENU
    static String getNavBar(String activePage) {
        String html = R"rawliteral(
<nav class="navbar">
  <a href="/" class="nav-brand"><span>🤖</span> MAJIN</a>
  <div class="nav-links">
    <a href="/" class=")rawliteral"; html += (activePage == "home") ? "active" : ""; html += R"rawliteral(">Dash</a>
    <a href="/tamagotchi" class=")rawliteral"; html += (activePage == "tamagotchi") ? "active" : ""; html += R"rawliteral(">Vie</a>
    <a href="/deck" class=")rawliteral"; html += (activePage == "deck") ? "active" : ""; html += R"rawliteral(">Deck</a>
    <a href="/countdown" class=")rawliteral"; html += (activePage == "countdown") ? "active" : ""; html += R"rawliteral(">Event</a>
    <a href="/modules" class=")rawliteral"; html += (activePage == "modules") ? "active" : ""; html += R"rawliteral(">Hardw</a>
    <a href="/perso" class=")rawliteral"; html += (activePage == "perso") ? "active" : ""; html += R"rawliteral(">Perso</a>
    <a href="/settings" class=")rawliteral"; html += (activePage == "settings") ? "active" : ""; html += R"rawliteral(">Config</a>
  </div>
</nav>
        )rawliteral";
        return html;
    }

    static String getFooter() { return "<div class='footer'>MAJIN OS v2.4 • Connected Intelligence</div>"; }
    
    static String getSetupInjectionJS() {
        return R"rawliteral(<script>window.onload=function(){var h=document.createElement('div');h.innerHTML='<div style="font-size:40px;text-align:center">🤖</div><h1 style="text-align:center;color:#00F0FF">MAJIN OS</h1>';document.body.insertBefore(h,document.body.firstChild);var w=document.createElement('div');w.style.cssText="background:#1a1a1a;padding:15px;color:#ccc;border-left:3px solid #00F0FF;margin-bottom:20px";w.innerHTML='<p><strong>Bienvenue !</strong><br>Connecte-moi au WiFi.</p>';var t=document.querySelector('form')||document.querySelector('button');if(t)t.parentNode.insertBefore(w,t);var b=document.querySelectorAll('button');b.forEach(function(x){if(x.innerText.toLowerCase().includes('wifi'))x.style.background="#00F0FF";});var i=document.getElementById('robot_name');if(i){i.required=true;i.placeholder="Nom du robot";}};</script>)rawliteral";
    }
};

#endif