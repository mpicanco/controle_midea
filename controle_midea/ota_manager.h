#pragma once
/**
 * @file ota_manager.h
 * @brief Atualizacao de firmware (OTA) via navegador.
 *
 * Particao: usa a Update API do core ESP32, que grava na particao
 * irma (app1) mantendo o firmware atual como fallback.
 *
 * Rotas registradas em servidor:
 *   GET  /ota        -> pagina de upload do .bin (estilo HUD)
 *   POST /otaupload  -> recebe o firmware (multipart) e reinicia
 *
 * O binario pode ser gerado com:
 *   arduino-cli compile --fqbn esp32:esp32:esp32 --build-path ./build controle_midea
 *   (o arquivo e ./build/controle_midea.ino.bin)
 */

#include <Arduino.h>
#include <WebServer.h>

/// Registra as rotas de OTA no servidor (chamar uma unica vez).
void ota_register(WebServer& server);
