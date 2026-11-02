#pragma once
/**
 * @file wifi_manager.h
 * @brief Gerenciamento de WiFi com provisionamento persistente (NVS).
 *
 * Fluxo de inicializacao:
 *   1. Le SSID/senha da flash (NVS, namespace NVS_NAMESPACE);
 *   2. Se houver rede salva -> tenta conectar em STA por STA_TIMEOUT_MS;
 *   3. Conectou: servidor web ativo no IP local (modo normal);
 *   4. Falhou / sem credenciais: abre AP AP_SSID com portal cativo
 *      (DNS repassa todo dominio para http://192.168.4.1/setup).
 *
 * A pagina de configuracao salva via POST /save; segurar o botao BOOT
 * (RESET_BTN_PIN) por RESET_BTN_MS apaga as credenciais e reinicia em
 * modo portal.
 */

#include <Arduino.h>

/// Le credenciais da NVS, conecta em STA ou abre o AP de configuracao.
/// Deve ser chamado a partir do setup(), antes de web_server_begin().
void wifi_begin();

/// Tarefas periodicas: DNS do portal, reconexao STA e botao BOOT.
/// Chame a partir do loop().
void wifi_loop();

/// true quando o ESP32 esta em modo AP de configuracao (portal).
bool wifi_isPortal();

/// true se existe SSID salvo na NVS (primeira conexao -> false).
bool wifi_hasCredentials();

/// Salva SSID/senha na NVS e reinicia.
void wifi_saveCredentials(const String& ssid, const String& pass);

/// JSON com informacoes da rede atual (uso interno/debug).
String wifi_statusJson();
