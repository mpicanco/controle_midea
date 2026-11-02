# MIDEA Remote — ESP32

Controle remoto IR para ar-condicionado **Midea** (e compatíveis) via **ESP32 WROOM DevKit V4**, com interface web estilo **HUD espacial** (dark mode, azul ciano, display 7 segmentos em CSS puro) e **provisionamento de WiFi sem código** — tudo configurado pelo celular na primeira conexão.

> Baseado na biblioteca `esp32_rmt_midea_ir_tx` (protocolo Midea via RMT do ESP32).

---

## 1. Funcionalidades

| Recurso | Descrição |
|---|---|
| Ligar / Desligar | Envia o estado completo via IR |
| Temperatura | 17 °C a 30 °C (limites físicos do protocolo Midea) |
| Modo | Alternância Frio ⇄ Quente |
| Oscilar | Comando de oscilação das palhetas (deflector) |
| Ventilação | Auto / 1 / 2 / 3 (suportado pela biblioteca) |
| WiFi persistente | Credenciais salvos na flash (NVS), portal de configuração no primeiro uso |
| Estado persistente | `on`, modo, **temperatura**, ventilação e **oscilação** salvos na NVS a cada comando; após reset o ajuste volta do último valor (padrão de fábrica: 23 °C, frio, vent. 1, palhetas fixas, desligado) |

## 2. Hardware

- **ESP32 DevKit V4** (WROOM-32)
- **LED emissor IR 940 nm + resistor ~100 Ω** no **GPIO 4**:
  - Direto: `GPIO4 → 100Ω → LED IR → GND` (alcance curto, ~2–3 m).
  - Com transistor (recomendado): NPN 2N2222 em modo keying com o LED conectado ao VCC, base via 100Ω no GPIO4.
- **Botão BOOT (GPIO0)**: vem embutido na placa; usado para resetar o WiFi.
- LED IR em linha de visão com o receptor do AC.

## 3. Provisionamento do WiFi (primeiro uso)

1. No boot sem rede salva, o ESP32 abre um Access Point aberto: **`Midea_Remote`** (`192.168.4.1`).
2. Conecte o celular — o **portal cativo** abre automaticamente a página de configuração (ou acesse `http://192.168.4.1/setup`).
3. Toque em **ESCANEAR REDES**, selecione sua rede, digite a senha e escolha **SALVAR E REINICIAR**.
4. As credenciais ficam gravadas na flash (NVS). Nos próximos boots o ESP32 conecta direto em modo cliente (STA) e imprime o IP no Serial Monitor (115200).
5. Se falhar ao conectar (rede ausente/errada), ele volta ao portal após ~20 s.

### 3.1 Acesso pelo navegador: alias mDNS

O dispositivo se anuncia na rede como **`air_office.local`** (mDNS/Bonjour, `ESPmDNS`) — em qualquer modo:

```
http://air_office.local
```

- **macOS/iOS/Windows 10+ e Linux (Avahi):** resolvem `.local` nativamente.
- **Android:** suporte a mDNS no navegador é irregular — nesse caso use o IP (veja abaixo).
- O hostname é definido em `config.h:DEVICE_HOSTNAME` e também aparece no painel do roteador (`WiFi.setHostname`).
- **IP fixo (recomendação complementar):** configure uma **reserva DHCP** no roteador para o MAC do ESP32 — combina bem com o alias e garante endereço estável.

**Resetar WiFi:** segure o botão **BOOT por 5 s** → credenciais apagadas, reinicia em modo portal.

> Mesmo no modo portal, o painel de controle fica acessível via `http://192.168.4.1/` (ou `http://air_office.local`, se o cliente resolver mDNS na rede do AP).

## 4. Estrutura do projeto

```
controle_midea/
├── controle_midea.ino      ← setup()/loop() (orquestração mínima)
├── config.h                ← pinos, rede do AP, timeouts (edite aqui)
├── midea_control.h/.cpp    ← estado do AC + conversão de comandos em sinais IR
├── wifi_manager.h/.cpp     ← NVS, STA/AP, portal cativo (DNS), botão BOOT
├── pages.h                 ← páginas HTML/CSS/JS (PROGMEM): painel HUD, setup WiFi e OTA
├── ota_manager.h/.cpp      ← atualização de firmware via navegador (partição OTA app1)
├── web_server.h/.cpp       ← rotas HTTP (UI + API JSON)
├── docs/
│   └── WEB_API.md          ← referência dos endpoints HTTP
└── README.md
```

### Arquitetura

```
Navegador (WiFi)                  ESP32                          Ar-condicionado
               ── HTTP ─►  web_server.cpp ── mc_command() ──► midea_ir_send()
                            (rotas /status   (estado,            midea_ir_oscilate()
                             /cmd/scan…)      labels)
                                │
                          wifi_manager.cpp
                          (STA/AP, NVS, portal cativo, BOOT)
```

- **Fluxo de comando:** botão na UI → `fetch("/cmd?act=…")` → `mc_command()` → biblioteca `midea_ir` → **RMT GPIO4** → LED IR.
- **Estado:** serializado por `mc_stateJson()`; a UI faz fetch inicial e polling a cada 5 s.

## 5. Endpoints da API

Referência completa em [`docs/WEB_API.md`](docs/WEB_API.md). Resumo:

| Método | Rota | Função |
|---|---|---|
| GET | `/` | Painel de controle (HUD) |
| GET | `/setup` | Página de configuração WiFi |
| POST | `/save` | Salva SSID/senha (`ssid`, `pass`) e reinicia |
| GET | `/scan` | JSON `[{ssid, rssi}]` das redes próximas |
| GET | `/status` | Estado: `{on, mode, temp, fan, last}` |
| GET | `/cmd?act=…&v=…` | `power`, `up`, `down`, `mode`, `osc`, `fan` |
| GET | `/ota` | Página de atualização de firmware (OTA) |
| POST | `/otaupload` | Recebe o `.bin` e reinicia no novo firmware |

## 6. Compilar e gravar

### 6.0 Script de build (`build.sh`)

```bash
./build.sh                  # compila -> ./build/controle_midea.ino.bin
./build.sh -u /dev/ttyUSB0  # compila + grava por cabo serial
./build.sh -u /dev/ttyUSB0 -S   # compila + grava + monitor serial (115200)
./build.sh -m 192.168.1.50  # compila + envia por OTA (endpoint /otaupload)
./build.sh -p               # lista portas seriais
./build.sh -c               # trabalho limpo (apaga ./build antes)
./build.sh -h               # ajuda
```

### Compilação manual com `arduino-cli`

```bash
arduino-cli compile --fqbn esp32:esp32:esp32 controle_midea

# veja a porta com: arduino-cli port list
arduino-cli upload -p /dev/ttyUSB0 --fqbn esp32:esp32:esp32 controle_midea
```

No **Arduino IDE**: abra o `controle_midea.ino` — os arquivos `.h`/`.cpp` da mesma pasta são compilados automaticamente.

**Dependência:** biblioteca `esp32_rmt_midea_ir_tx` instalada em `~/Arduino/libraries/`.

### 6.1 OTA (atualizar firmware sem cabo)

O esquema de partições padrão tem uma partição irmã (app1) do mesmo tamanho do
firmware — o código reside na `app0` e a `app1` é usada como destino de gravação
(fallback se algo deu errado).

`./build.sh -m <ip>` faz o ciclo completo (compilar + enviar). Manualmente:

1. Compile o binário:

```bash
arduino-cli compile --fqbn esp32:esp32:esp32 --build-path ./build controle_midea
# binário: ./build/controle_midea.ino.bin
```

2. No celular/PC (mesmo WiFi), abra `http://<ip-do-esp32>/ota` (ou link **📥 OTA** no rodapé do painel);
3. Selecione o `.bin` → **⚡ ATUALIZAR FIRMWARE**;
4. O ESP32 reinicia no novo firmware; o ajuste salvo (temperatura/modo/vent.) é mantido.

> Primeira instalação deve ser por cabo (USB); após isso, todas as próximas podem ser via OTA.

## 7. Protocolo Midea (referência)

Pacote de 3 bytes: `[0xB2] [ffff ssss] [tttt cccc]`, cada byte seguido do inverso, transmitido em par:

| Campo | Bits | Valores |
|---|---|---|
| `ffff` | ventilação | 1011 auto, 1001 baixa, 0101 média, 0011 alta |
| `ssss` | estado | 1111 ligado, 1011 desligado |
| `tttt` | temperatura | 0000=17 °C … 1011=30 °C |
| `cccc` | comando | 0000 frio, 1100 quente, 1000 auto |

Comandos especiais (frames independentes): oscilação `{0xB2, 0x6B, 0xE0}`, deflector, turbo, ionizador etc. — ver `midea_ir.c`.

## 8. Solução de problemas

| Sintoma | Causa provável / solução |
|---|---|
| AC não responde | Verificar LED em linha de visão e polaridade do diodo IR |
| Portal não abre no celular | Acesse `http://192.168.4.1/setup` (alguns Androids não disparam o captive portal) |
| Não entra em portal | Há rede salva funcionando; segure BOOT por 5 s |
| Falha ao conectar após salvar | Refaça o portal via BOOT 5 s (senha errada) |
| UI não carrega | Confira o IP no Serial Monitor; celular e ESP32 devem estar na mesma rede |
