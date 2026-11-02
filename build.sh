#!/usr/bin/env bash
# ==============================================================================
#  build.sh — script de build/implantacao do projeto controle_midea (ESP32)
#
#  Uso:
#    ./build.sh                  compila e gera o binario em ./build/
#    ./build.sh -u /dev/ttyUSB0  compila + grava pela porta serial
#    ./build.sh -m 192.168.x.x   compila + envia via OTA para o ESP32
#    ./build.sh -S               abre o monitor serial apos o upload
#    ./build.sh -p               lista as portas seriais
#    ./build.sh -c               limpa ./build/ antes de compilar
#
#  Onde fica o binario final (o "servico" do OTA e este arquivo):
#    ./build/controle_midea.ino.bin
# ==============================================================================
set -euo pipefail

SKETCH_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)/controle_midea"
BUILD_DIR="$(pwd)/build"
FQBN="esp32:esp32:esp32"

PORT=""; OTA_HOST=""; DO_MONITOR=0; DO_PORTS=0; CLEAN=0

usage() { sed -n '3,14p' "$0"; exit 0; }

# resolve o arduino-cli (no flatpak da Arduino IDE ou no PATH)
resolve_cli() {
    if command -v arduino-cli >/dev/null 2>&1; then
        CLI="$(command -v arduino-cli)"
    elif [ -f /var/lib/flatpak/app/cc.arduino.IDE2/current/active/files/arduino-ide/resources/app/lib/backend/resources/arduino-cli ]; then
        CLI="/var/lib/flatpak/app/cc.arduino.IDE2/current/active/files/arduino-ide/resources/app/lib/backend/resources/arduino-cli"
    else
        echo "ERRO: arduino-cli nao encontrado (PATH ou instalacao da Arduino IDE)." >&2
        exit 1
    fi
}

while getopts "hSpSu:m:c" opt; do
    case $opt in
        h) usage ;;
        S) DO_MONITOR=1 ;;
        p) DO_PORTS=1 ;;
        u) PORT="$OPTARG" ;;
        m) OTA_HOST="$OPTARG" ;;
        c) CLEAN=1 ;;
        *) usage ;;
    esac
done

resolve_cli

# --- listar portas ---
if [ "$DO_PORTS" = "1" ]; then
    exec "$CLI" port list
fi

# --- limpeza ---
if [ "$CLEAN" = "1" ]; then
    echo "[build] limpando $BUILD_DIR ..."
    rm -rf "$BUILD_DIR"
fi

# --- compilacao ---
echo "[build] compilando (fqbn=$FQBN) ..."
"$CLI" --config-file "$HOME/.arduino15/arduino-cli.yaml" \
    compile --fqbn "$FQBN" --build-path "$BUILD_DIR" "$SKETCH_DIR"

BIN="$BUILD_DIR/controle_midea.ino.bin"
echo "[build] OK -> $BIN"

# --- upload serial ---
if [ -n "$PORT" ]; then
    echo "[build] gravando via serial em $PORT ..."
    "$CLI" --config-file "$HOME/.arduino15/arduino-cli.yaml" \
        upload -p "$PORT" --fqbn "$FQBN" --input-dir "$BUILD_DIR" "$SKETCH_DIR"
    echo "[build] gravado com sucesso."
    if [ "$DO_MONITOR" = "1" ]; then
        exec "$CLI" monitor -p "$PORT" --config 115200
    fi
    exit 0
fi

# --- upload OTA ---
if [ -n "$OTA_HOST" ]; then
    case "$OTA_HOST" in
        http*) BASE="$OTA_HOST" ;;
        *)     BASE="http://$OTA_HOST" ;;
    esac
    echo "[ota] enviando firmware para $BASE/otaupload ..."
    HTTP_CODE="$(curl -s -o /tmp/opencode/ota_reply.html -w '%{http_code}' \
        -F "fw=@$BIN" "$BASE/otaupload")" || {
        echo "[ota] ERRO: falha de conexao com o ESP32." >&2
        exit 1;
    }
    if [ "$HTTP_CODE" = "200" ]; then
        # o texto de resposta e o ESP32 rebootando em seguida
        cat /tmp/opencode/ota_reply.html | tr -d '\0'
        echo; echo "[ota] firmware enviado; ESP32 reiniciando..."
    else
        echo "[ota] ERRO: HTTP $HTTP_CODE. Resposta:" >&2
        cat /tmp/opencode/ota_reply.html >&2
        exit 1
    fi
fi

echo "[build] pronto."
