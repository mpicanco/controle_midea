#pragma once
/**
 * @file pages.h
 * @brief Paginas web embarcadas (PROGMEM).
 *
 *  - INDEX_HTML : painel de controle HUD (7 segmentos, dark mode)
 *  - SETUP_HTML : formulario de configuracao WiFi (portal cativo)
 *  - SAVE_OK    : resposta apos gravar credenciais (antes do reboot)
 */

#ifndef PAGES_H
#define PAGES_H

const char INDEX_HTML[] PROGMEM = R"HTML(<!DOCTYPE html>
<html lang="pt-BR">
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width,initial-scale=1">
<title>MIDEA // CLIMATE CONTROL</title>
<style>
:root{
  --hud:#4dd8ff; --hud-dim:#1a6a8a; --glow:rgba(77,216,255,.55);
  --bg0:#02060d; --bg1:#061423; --panel:rgba(10,30,48,.55);
  --off:rgba(77,216,255,.08);
  --hot:#ff9a4d; --cool:#4dd8ff;
}
*{box-sizing:border-box;margin:0;padding:0;user-select:none}
html,body{height:100%}
body{
  font-family:"Share Tech Mono","Courier New",monospace;
  color:var(--hud); background:
    radial-gradient(1200px 600px at 50% -10%, #0a2238 0%, transparent 60%),
    linear-gradient(180deg, var(--bg0), var(--bg1) 70%, var(--bg0));
  min-height:100vh; display:flex; align-items:center; justify-content:center;
  padding:12px; overflow-x:hidden;
}
body::after{ /* scanlines */
  content:""; position:fixed; inset:0; pointer-events:none;
  background:repeating-linear-gradient(0deg, rgba(160,230,255,.035) 0 1px, transparent 1px 3px);
}
.frame{
  position:relative; max-width:560px; width:100%;
  border:1px solid rgba(77,216,255,.25);
  background:linear-gradient(180deg, rgba(8,24,40,.85), rgba(4,12,22,.9));
  box-shadow:0 0 24px rgba(77,216,255,.12), inset 0 0 40px rgba(0,20,40,.6);
  padding:22px 20px 26px; border-radius:6px;
}
/* cantos de HUD */
.corner{position:absolute; width:22px; height:22px; border:2px solid var(--hud); opacity:.85}
.c1{top:-2px;left:-2px;border-right:0;border-bottom:0}
.c2{top:-2px;right:-2px;border-left:0;border-bottom:0}
.c3{bottom:-2px;left:-2px;border-right:0;border-top:0}
.c4{bottom:-2px;right:-2px;border-left:0;border-top:0}

.head{display:flex;justify-content:space-between;align-items:center;letter-spacing:2px;
  font-size:14px;border-bottom:1px dashed rgba(77,216,255,.25);padding-bottom:10px;margin-bottom:14px}
.head .blink{animation:bl 1.2s infinite}
@keyframes bl{0%,60%{opacity:1}61%,100%{opacity:.15}}
.dot{width:10px;height:10px;border-radius:50%;background:#0f0;box-shadow:0 0 8px #0f0;display:inline-block;margin-right:8px;vertical-align:-1px}
.dot.dead{background:#f55;box-shadow:0 0 8px #f55}

/* --------- display 7 segmentos --------- */
.disp{
  display:flex;align-items:center;justify-content:center;gap:14px;margin:6px 0 4px;
  transform:skewX(-5deg);
}
.dig{position:relative;width:74px;height:132px}
.dig.dim .seg.on{opacity:.22}
.seg{position:absolute;border-radius:4px;background:var(--hud);
  box-shadow:0 0 10px var(--glow),0 0 26px rgba(77,216,255,.35);
  opacity:var(--off); transition:opacity .1s}
.seg.on{opacity:1}
.h{left:14%;right:14%;height:11%}
.v{width:11%;top:8%;height:36%}
.v2{width:11%;top:56%;height:36%}
.sa{top:0}.sg{top:44.5%}.sd{bottom:0}
.sb{right:0}.sc{right:0}.sf{left:0}.se{left:0}
.unit{text-align:center;letter-spacing:3px;font-size:14px;color:var(--hud-dim);margin-bottom:14px}
.unit b{color:var(--hud);text-shadow:0 0 8px var(--glow);font-size:18px}

/* --------- barra de estado --------- */
.status{display:flex;justify-content:center;gap:10px;flex-wrap:wrap;margin-bottom:16px}
.chip{border:1px solid rgba(77,216,255,.3);padding:5px 12px;font-size:12px;letter-spacing:2px;
  background:rgba(10,40,60,.35);color:var(--hud-dim);border-radius:3px}
.chip.on{color:#062231;background:var(--hud);box-shadow:0 0 14px var(--glow);border-color:var(--hud)}
.chip.hot.on{background:var(--hot);box-shadow:0 0 14px rgba(255,154,77,.5);border-color:var(--hot)}

/* --------- controles --------- */
.grid{display:grid;gap:10px;grid-template-columns:1fr 1fr}
button{
  font-family:inherit;font-size:15px;letter-spacing:2px;padding:14px 8px;cursor:pointer;
  background:linear-gradient(180deg, rgba(20,60,90,.35), rgba(8,26,42,.6));
  color:var(--hud); border:1px solid rgba(77,216,255,.4); border-radius:4px;
  transition:all .12s; position:relative;
}
button:hover{border-color:var(--hud);box-shadow:0 0 14px rgba(77,216,255,.3);background:rgba(20,70,105,.45)}
button:active{transform:translateY(2px)}
button.wide{grid-column:1/-1}
button.small{padding:9px 4px;font-size:13px}
button.active{background:var(--hud);color:#03202e;box-shadow:0 0 16px var(--glow);border-color:var(--hud);font-weight:bold}
.btn-pw button.active{background:#3bff8f;border-color:#3bff8f;box-shadow:0 0 16px rgba(59,255,143,.5);color:#02280f}
.lbl{font-size:10px;color:var(--hud-dim);letter-spacing:3px;display:block;margin-bottom:9px;text-align:center}
.fanrow{display:grid;grid-template-columns:repeat(4,1fr);gap:8px;margin-bottom:12px}
.log{margin-top:16px;text-align:center;font-size:12px;color:var(--hud-dim);letter-spacing:2px;
  border-top:1px dashed rgba(77,216,255,.2);padding-top:10px}
.log b{color:var(--hud);text-shadow:0 0 8px var(--glow)}
.rails{display:flex;justify-content:space-between;color:var(--hud-dim);font-size:10px;letter-spacing:2px;margin-top:14px;opacity:.7}
.rails a{color:var(--hud-dim);text-decoration:none;border-bottom:1px dotted rgba(77,216,255,.4)}
.rails a:hover{color:var(--hud)}
</style>
</head>
<body>
<div class="frame">
  <div class="corner c1"></div><div class="corner c2"></div>
  <div class="corner c3"></div><div class="corner c4"></div>

  <div class="head">
    <span><span class="dot" id="dot"></span>MIDEA // CLIMATE CTRL</span>
    <span class="blink">_</span>
  </div>

  <div class="disp">
    <div class="dig" id="d0"></div>
    <div class="dig" id="d1"></div>
  </div>
  <div class="unit">TEMPERATURA AJUSTADA &nbsp;<b>&deg;C</b></div>

  <div class="status">
    <span class="chip" id="chipPw">DESLIGADO</span>
    <span class="chip" id="chipMode">FRIO</span>
    <span class="chip" id="chipFan">VENT AUTO</span>
  </div>

  <div class="grid btn-pw">
    <button id="bPower" class="wide" onclick="cmd('power')">&#9211; LIGAR / DESLIGAR</button>
  </div>

  <div style="height:12px"></div>
  <span class="lbl">- TEMPERATURA +</span>
  <div class="grid">
    <button onclick="cmd('down')">&#9660; DIMINUIR</button>
    <button onclick="cmd('up')">&#9650; AUMENTAR</button>
  </div>

  <div style="height:12px"></div>
  <span class="lbl">MODO DE OPERACAO</span>
  <div class="grid">
    <button id="bCool" onclick="setMode('cool')">&#10052;&#65039; FRIO</button>
    <button id="bHeat" onclick="setMode('heat')">&#9728;&#65039; QUENTE</button>
  </div>

  <div style="height:12px"></div>
  <span class="lbl">VENTILACAO</span>
  <div class="fanrow">
    <button class="small" id="f0" onclick="cmd('fan',0)">AUTO</button>
    <button class="small" id="f1" onclick="cmd('fan',1)">1</button>
    <button class="small" id="f2" onclick="cmd('fan',2)">2</button>
    <button class="small" id="f3" onclick="cmd('fan',3)">3</button>
  </div>

  <div class="grid">
    <button class="wide" onclick="cmd('osc')">&#9784;&#65039; OSCILAR PALHETAS</button>
  </div>

  <div class="log">CMD&gt; <b id="log">INIT</b></div>
  <div class="rails"><span>IR TX GPIO4</span><span>SYS OK</span><a href="/ota">&#128230; OTA</a>&nbsp;<a href="/setup">&#9881; CFG</a></div>
</div>

<script>
const SEGS={ /* segmentos a..g por digito */
  0:"abcdef",1:"bc",2:"abged",3:"abgcd",4:"fgbc",5:"afgcd",
  6:"afgecd",7:"abc",8:"abcdefg",9:"abcdfg"};
const DASH="g";

function buildDig(el){
  el.innerHTML="";
  const s={};
  for(const k of ["a","b","c","d","e","f","g"]){
    const d=document.createElement("div");
    d.classList.add("seg");
    if(k==="a"||k==="g"||k==="d") d.classList.add("h");
    else d.classList.add("v");
    if(k==="c"||k==="e") d.classList.add("v2");
    d.classList.add("s"+k);
    el.appendChild(d); s[k]=d;
  }
  return s;
}
function draw(el,ch){
  if(!el._s){el._s=buildDig(el);}
  const on=(ch=="-"?DASH:SEGS[ch])||"";
  for(const k of "abcdefg") el._s[k].classList.toggle("on", on.includes(k));
}
let st={on:false,mode:"cool",temp:24,fan:0};

function render(){
  // setpoint SEMPRE visivel (apagado = digitos dimmed)
  const t=st.temp.toString().padStart(2,"0").split("");
  draw(document.getElementById("d0"), t[0]);
  draw(document.getElementById("d1"), t[1]);
  document.getElementById("d0").classList.toggle("dim", !st.on);
  document.getElementById("d1").classList.toggle("dim", !st.on);
  document.getElementById("bPower").classList.toggle("active", st.on);
  const cp=document.getElementById("chipPw");
  cp.textContent=st.on?"LIGADO":"DESLIGADO"; cp.classList.toggle("on",st.on);
  const cm=document.getElementById("chipMode");
  cm.textContent=st.mode==="heat"?"QUENTE":"FRIO"; cm.classList.toggle("hot",st.mode==="heat");
  cm.classList.toggle("on",st.on);
  document.getElementById("bCool").classList.toggle("active", st.on&&st.mode==="cool");
  document.getElementById("bHeat").classList.toggle("active", st.on&&st.mode==="heat");
  document.getElementById("chipFan").textContent="VENT "+(st.fan==0?"AUTO":st.fan);
  document.getElementById("chipFan").classList.toggle("on",st.on);
  for(let i=0;i<4;i++) document.getElementById("f"+i).classList.toggle("active", st.on&&st.fan==i);
  document.getElementById("log").textContent=st.last||"-";
  document.getElementById("dot").classList.toggle("dead",!st.on);
}
async function refresh(){
  try{ st=await (await fetch("/status")).json(); render(); }
  catch(e){ document.getElementById("log").textContent="ERRO DE CONEXAO"; }
}
async function cmd(act,v){
  try{
    st=await (await fetch("/cmd?act="+act+(v!==undefined?"&v="+v:""))).json();
    render();
  }catch(e){ document.getElementById("log").textContent="ERRO DE CONEXAO"; }
}
function setMode(m){ if(st.mode!==m) cmd('mode'); }
refresh(); setInterval(refresh,5000);
</script>
</body>
</html>
)HTML";

const char SETUP_HTML[] PROGMEM = R"SETUP(<!DOCTYPE html>
<html lang="pt-BR">
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width,initial-scale=1">
<title>MIDEA // CONFIG WIFI</title>
<style>
:root{--hud:#4dd8ff;--hud-dim:#1a6a8a;--glow:rgba(77,216,255,.55)}
*{box-sizing:border-box;margin:0;padding:0}
body{font-family:"Share Tech Mono","Courier New",monospace;color:var(--hud);
  background:linear-gradient(180deg,#02060d,#061423 70%,#02060d);
  min-height:100vh;display:flex;align-items:center;justify-content:center;padding:12px}
body::after{content:"";position:fixed;inset:0;pointer-events:none;
  background:repeating-linear-gradient(0deg,rgba(160,230,255,.035) 0 1px,transparent 1px 3px)}
.frame{position:relative;max-width:420px;width:100%;border:1px solid rgba(77,216,255,.25);
  background:linear-gradient(180deg,rgba(8,24,40,.85),rgba(4,12,22,.9));
  box-shadow:0 0 24px rgba(77,216,255,.12),inset 0 0 40px rgba(0,20,40,.6);
  padding:24px 22px;border-radius:6px}
.corner{position:absolute;width:22px;height:22px;border:2px solid var(--hud)}
.c1{top:-2px;left:-2px;border-right:0;border-bottom:0}
.c2{top:-2px;right:-2px;border-left:0;border-bottom:0}
.c3{bottom:-2px;left:-2px;border-right:0;border-top:0}
.c4{bottom:-2px;right:-2px;border-left:0;border-top:0}
h1{font-size:15px;letter-spacing:3px;border-bottom:1px dashed rgba(77,216,255,.25);
  padding-bottom:10px;margin-bottom:6px}
.sub{font-size:11px;color:var(--hud-dim);letter-spacing:2px;margin-bottom:18px}
label{display:block;font-size:10px;color:var(--hud-dim);letter-spacing:3px;margin:14px 0 6px}
input{width:100%;font-family:inherit;font-size:16px;color:var(--hud);
  background:rgba(6,26,44,.7);border:1px solid rgba(77,216,255,.35);border-radius:4px;
  padding:12px 10px;outline:none;letter-spacing:1px}
input:focus{border-color:var(--hud);box-shadow:0 0 12px rgba(77,216,255,.25)}
button{font-family:inherit;font-size:14px;letter-spacing:2px;padding:13px;width:100%;
  margin-top:18px;cursor:pointer;color:var(--hud);
  background:linear-gradient(180deg,rgba(20,60,90,.35),rgba(8,26,42,.6));
  border:1px solid rgba(77,216,255,.4);border-radius:4px;transition:all .12s}
button:hover{border-color:var(--hud);box-shadow:0 0 14px rgba(77,216,255,.3)}
button:active{transform:translateY(2px)}
button.save{background:var(--hud);color:#03202e;font-weight:bold;border-color:var(--hud)}
#scanOut{margin-top:12px;font-size:12px;color:var(--hud-dim);max-height:130px;overflow:auto}
#scanOut div{padding:6px 4px;border-bottom:1px dashed rgba(77,216,255,.15);cursor:pointer}
#scanOut div:hover{color:var(--hud);background:rgba(20,60,90,.25)}
.msg{text-align:center;font-size:12px;color:var(--hud-dim);min-height:18px;margin-top:12px;letter-spacing:1px}
</style>
</head>
<body>
<div class="frame">
  <div class="corner c1"></div><div class="corner c2"></div>
  <div class="corner c3"></div><div class="corner c4"></div>
  <h1>MIDEA // CONFIG WIFI</h1>
  <div class="sub">MODO DE CONFIGURACAO &mdash; REDE: Midea_Remote</div>

  <label>SSID DA REDE</label>
  <input id="ssid" autocomplete="off" placeholder="nome da sua rede">
  <label>SENHA</label>
  <input id="pass" type="password" placeholder="senha da sua rede">

  <button onclick="salvar()" class="save">&#9588; SALVAR E REINICIAR</button>
  <button onclick="escanear()">&#9776; ESCANEAR REDES</button>
  <div id="scanOut"></div>
  <div class="msg" id="msg"></div>
</div>
<script>
async function escanear(){
  const m=document.getElementById("msg");
  m.textContent="ESCANEANDO...";
  try{
    const r=await fetch("/scan"); const nets=await r.json();
    const box=document.getElementById("scanOut"); box.innerHTML="";
    nets.sort((a,b)=>b.rssi-a.rssi).forEach(n=>{
      const d=document.createElement("div");
      d.textContent=(n.rssi>-70?"\u25CF ":"\u25CB ")+n.ssid;
      d.onclick=()=>{document.getElementById("ssid").value=n.ssid;
        document.getElementById("msg").textContent="REDE SELECIONADA: "+n.ssid;};
      box.appendChild(d);
    });
    if(!nets.length) box.innerHTML="<div>NENHUMA REDE ENCONTRADA</div>";
    m.textContent="REDES ENCONTRADAS";
  }catch(e){ m.textContent="ERRO AO ESCANEAR"; }
}
async function salvar(){
  const s=document.getElementById("ssid").value.trim();
  const p=document.getElementById("pass").value;
  if(!s){document.getElementById("msg").textContent="INFORME O SSID";return;}
  const f=new FormData(); f.append("ssid",s); f.append("pass",p);
  await fetch("/save",{method:"POST",body:f});
  document.getElementById("msg").textContent="SALVO! REINICIANDO...";
}
</script>
</body>
</html>
)SETUP";

const char SAVE_OK[] PROGMEM =
    "<html><body style='background:#02060d;color:#4dd8ff;font-family:monospace;"
    "text-align:center;padding-top:40vh;letter-spacing:2px'>"
    "SALVO!<br>REINICIANDO...</body></html>";

// ------------------------------------------------------------
//  PAGINA DE OTA (atualizacao de firmware via navegador)
// ------------------------------------------------------------
const char OTA_HTML[] PROGMEM = R"OTA(<!DOCTYPE html>
<html lang="pt-BR">
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width,initial-scale=1">
<title>MIDEA // OTA</title>
<style>
:root{--hud:#4dd8ff;--hud-dim:#1a6a8a;--glow:rgba(77,216,255,.55)}
*{box-sizing:border-box;margin:0;padding:0}
body{font-family:"Share Tech Mono","Courier New",monospace;color:var(--hud);
  background:linear-gradient(180deg,#02060d,#061423 70%,#02060d);
  min-height:100vh;display:flex;align-items:center;justify-content:center;padding:12px}
body::after{content:"";position:fixed;inset:0;pointer-events:none;
  background:repeating-linear-gradient(0deg,rgba(160,230,255,.035) 0 1px,transparent 1px 3px)}
.frame{position:relative;max-width:420px;width:100%;border:1px solid rgba(77,216,255,.25);
  background:linear-gradient(180deg,rgba(8,24,40,.85),rgba(4,12,22,.9));
  box-shadow:0 0 24px rgba(77,216,255,.12),inset 0 0 40px rgba(0,20,40,.6);
  padding:24px 22px;border-radius:6px}
.corner{position:absolute;width:22px;height:22px;border:2px solid var(--hud)}
.c1{top:-2px;left:-2px;border-right:0;border-bottom:0}
.c2{top:-2px;right:-2px;border-left:0;border-bottom:0}
.c3{bottom:-2px;left:-2px;border-right:0;border-top:0}
.c4{bottom:-2px;right:-2px;border-left:0;border-top:0}
h1{font-size:15px;letter-spacing:3px;border-bottom:1px dashed rgba(77,216,255,.25);
  padding-bottom:10px;margin-bottom:6px}
.sub{font-size:11px;color:var(--hud-dim);letter-spacing:2px;margin-bottom:18px}
.filewrap{border:1px dashed rgba(77,216,255,.4);border-radius:4px;padding:22px 10px;
  text-align:center;font-size:13px;cursor:pointer;transition:all .12s}
.filewrap:hover{border-color:var(--hud);box-shadow:0 0 14px rgba(77,216,255,.25)}
input[type=file]{display:none}
button{font-family:inherit;font-size:14px;letter-spacing:2px;padding:13px;width:100%;
  margin-top:18px;cursor:pointer;color:var(--hud);
  background:linear-gradient(180deg,rgba(20,60,90,.35),rgba(8,26,42,.6));
  border:1px solid rgba(77,216,255,.4);border-radius:4px;transition:all .12s}
button:hover{border-color:var(--hud);box-shadow:0 0 14px rgba(77,216,255,.3)}
button.flash{background:var(--hud);color:#03202e;font-weight:bold;border-color:var(--hud)}
#file{word-break:break-all;margin-top:10px;color:var(--hud);text-shadow:0 0 8px var(--glow);min-height:20px}
.msg{text-align:center;font-size:11px;color:var(--hud-dim);margin-top:16px;letter-spacing:1px;line-height:1.5}
</style>
</head>
<body>
<div class="frame">
  <div class="corner c1"></div><div class="corner c2"></div>
  <div class="corner c3"></div><div class="corner c4"></div>
  <h1>MIDEA // OTA UPDATE</h1>
  <div class="sub">FLASH VIA REDE &mdash; SEM CABO</div>

  <form id="f" action="/otaupload" method="POST" enctype="multipart/form-data" onsubmit="go()">
    <label class="filewrap" for="fw">
      TOQUE PARA ESCOLHER<br>O ARQUIVO .bin
      <input type="file" id="fw" name="fw" accept=".bin" required>
    </label>
    <div id="file"></div>
    <button class="flash" type="submit" id="btn">&#9889; ATUALIZAR FIRMWARE</button>
  </form>
  <div class="msg">O ESP32 reinicia ap&oacute;s gravar.<br>Se algo falhar, o firmware anterior continua ativo (fallback).</div>
</div>
<script>
document.getElementById("fw").addEventListener("change",e=>{
  document.getElementById("file").textContent=e.target.files[0]?.name||"";
});
function go(){
  const name=document.getElementById("file").value;
  if(!name){return false;}
  const b=document.getElementById("btn");
  b.disabled=true; b.textContent="ENVIANDO... AGUARDE";
  return true;
}
</script>
</body>
</html>
)OTA";

#endif
