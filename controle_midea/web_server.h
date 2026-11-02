#pragma once
/**
 * @file web_server.h
 * @brief Servidor HTTP: rotas da UI, da API de estado/comandos e do portal.
 */

#include <Arduino.h>

/// Registra todas as rotas e inicia o servidor (porta 80).
/// Chamar depois de wifi_begin() escolher o modo (portal ou cliente).
void web_server_begin();

/// Processa clientes HTTP (chame a partir do loop()).
void web_server_loop();
