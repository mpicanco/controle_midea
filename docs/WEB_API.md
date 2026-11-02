# API Web — MIDEA Remote

Todas as rotas respondem HTTP na porta 80, em **`http://air_office.local`** (alias
mDNS configurado em `DEVICE_HOSTNAME`; Android pode exigir o IP direto) ou no IP
impresso no Serial Monitor. No modo portal (AP de configuração), o ESP32 fica
em `192.168.4.1`.

---

## Páginas (HTML)

### `GET /`
Painel de controle HUD (7 segmentos, dark mode).

- No **modo portal** com **nenhuma rede salva**: `/` redireciona para a página de configuração.
- Nos demais casos: painel de controle.
- No modo STA com falha de conexão, a rota desconhecida responde 404 (sem redirecionamento).

### `GET /setup`
Página de configuração WiFi (scan de redes + SSID/senha), disponível em qualquer modo.

## Provisionamento

### `POST /save`
Salva as credenciais na NVS e reinicia o ESP32 em modo cliente.

| Parâmetro (form) | Tipo  | Descrição |
|------------------|-------|-----------|
| `ssid`           | texto | Nome da rede (obrigatório) |
| `pass`           | texto | Senha (vazio = rede aberta) |

- 200 → HTML "SALVO! REINICIANDO..." (o ESP32 reinicia ~1 s depois)
- 400 → `SSID vazio`

### `GET /scan`
Dispara (se necessário) e/ou retorna a varredura de redes 2.4 GHz.

```json
[
  {"ssid": "MinhaRede", "rssi": -42},
  {"ssid": "OutraRede", "rssi": -78}
]
```

Pode retornar `[]` enquanto a varredura assíncrona está em andamento — repita a chamada.

---

## Atualização de firmware (OTA)

### `GET /ota`
Página HTML de upload (campo de arquivo `.bin` + botão). O firmware é gravado
na partição OTA (`app1`) com fallback automático para a versão atual em caso
de falha.

### `POST /otaupload`
`multipart/form-data` com o arquivo. Após receber todos os chunks, o ESP32:

- `200` + "FIRMWARE RECEBIDO! REINICIANDO..." e reinicia (~1 s) no novo firmware;
- `500` + "FALHA NO OTA!" se a gravação falhar (o firmware atual segue ativo).

Geração do `.bin`:

```
arduino-cli compile --fqbn esp32:esp32:esp32 --build-path ./build controle_midea
# ./build/controle_midea.ino.bin
```

---

## Estado e comandos

### `GET /status`
JSON do estado atual do ar-condicionado:

```json
{
  "on": true,
  "mode": "cool",   // "cool" | "heat"
  "temp": 24,        // 17..30
  "fan": 1,          // 0=Auto, 1..3
  "osc": false,      // toggle estimado das palhetas (sem feedback IR)
  "last": "TEMP-23"  // rótulo do último comando aplicado
}
```

O estado completo é persistente (NVS): cada comando grava `on/mode/temp/fan/osc`
na flash e é restaurado no próximo boot. Valores de fábrica: desligado,
frio, 23 °C, ventilação 1, palhetas fixas.

### `GET /cmd`
Aplica um comando e responde com o novo estado (`/status`). Formato:

```
/cmd?act=<acao>[&v=<0..3>]
```

| `act`   | `v`         | Efeito |
|---------|-------------|--------|
| `power` | —           | Alterna ligado/desligado |
| `up`    | —           | Aumenta temperatura (até 30 °C) |
| `down`  | —           | Diminui temperatura (até 17 °C) |
| `mode`  | —           | Alterna frio ⇄ quente |
| `osc`   | —           | Altera toggle das palhetas e envia frame `{0xB2,0x6B,0xE0}` |
| `fan`   | `0`,`1`,`2`,`3` | Ventilação (0 = auto) |

- Ação aplicada → `200` + JSON de estado.
- Ação desconhecida ou `fan` fora da faixa → `400` (`comando invalido`).

Exemplo:

```
curl "http://<ip>/cmd?act=fan&v=2"
```

---

## Informações da rede (debug)

### `GET` em rota inexistente
- Modo portal: `302` → `http://192.168.4.1/setup` (comportamento de captive portal).
- Modo STA: `404 Not Found`.
