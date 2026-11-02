#pragma once
/**
 * @file config.h
 * @brief Constantes de hardware e configuracao geral do projeto.
 *
 * Centraliza pinos, rede do AP de provisionamento, timeouts e limites
 * usados por todos os modulos.
 */

// ---------------- IR ----------------
#define MIDEA_RMT_CHANNEL   0       ///< canal RMT do ESP32 usado para o TX
#define MIDEA_TX_PIN        4       ///< GPIO do LED emissor IR (940nm)

// ---------------- Comandos do ar-condicionado ----------------
#define TEMP_MIN            17      ///< temperatura minima suportada (Celsius)
#define TEMP_MAX            30      ///< temperatura maxima suportada (Celsius)

// Estado inicial ao ligar o ESP32 (quando nao ha nada salvo na NVS)
#define DEFAULT_MODE        MODE_COOL
#define DEFAULT_FAN_LEVEL   1
#define DEFAULT_TEMP        23

// ---------------- WiFi (provisionamento) ----------------
#define AP_SSID             "Midea_Remote"  ///< nome do AP de configuracao
#define AP_PASS             ""              ///< SENHA do AP ("" = rede aberta; >=8 chars se quiser protege)

#define NVS_NAMESPACE       "midea"         ///< namespace na flash (NVS) para as credenciais WiFi
#define NVS_STATE_NAMESPACE "mstate"        ///< namespace para o estado persistente do AC
#define NVS_KEY_SSID        "ssid"
#define NVS_KEY_PASS        "pass"

// ---------------- Tempos ----------------
#define STA_TIMEOUT_MS      20000   ///< tempo tentando conectar em STA antes de abrir o AP
#define WIFI_RECONNECT_MS   5000    ///< intervalo entre tentativas de reconexao STA
#define RESET_BTN_MS        5000    ///< segurar o botao BOOT por este tempo apaga credenciais

// ---------------- Botoes ----------------
#define RESET_BTN_PIN       0       ///< GPIO0 = botao BOOT no DevKit

// ---------------- Rede / acesso ----------------
#define DEVICE_HOSTNAME     "air_office"    ///< alias mDNS: http://<hostname>.local V4
