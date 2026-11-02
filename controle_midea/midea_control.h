#pragma once
/**
 * @file midea_control.h
 * @brief Camada de controle do ar-condicionado Midea via IR.
 *
 * Encapsula o estado do sistema (on/off, modo, temperatura, ventilação),
 * converte comandos da interface web em sinais IR usando a biblioteca
 * esp32_rmt_midea_ir_tx e gera o JSON de estado consumido pela UI.
 *
 * O estado (on, modo, temperatura, ventilação) é PERSISTENTE: cada comando
 * é gravado na flash (NVS, namespace NVS_NAMESPACE) e restaurado no boot.
 *
 * Comandos aceitos por mc_command():
 *   - "power" : alterna ligado/desligado
 *   - "up"    : aumenta temperatura (ate TEMP_MAX)
 *   - "down"  : diminui temperatura (ate TEMP_MIN)
 *   - "mode"  : alterna frio/quente
 *   - "osc"   : oscila as palhetas (deflector)
 *   - "fan"   : nivel de ventilacao 0..3 (0 = auto)
 */

#include <Arduino.h>

/// Inicializa o modulo IR/estado com os valores de config.h.
void mc_init();

/**
 * @brief Aplica um comando vindo da interface.
 * @param act acao ("power", "up", "down", "mode", "osc" ou "fan")
 * @param v   valor associado (usado apenas por "fan": 0..3)
 * @return true se a acao foi implementada, false se desconhecida
 */
bool mc_command(const String& act, int v);

/// Serializa o estado atual para JSON (usado por /status e /cmd).
String mc_stateJson();

/// Devolve o ultimo comando processado (texto exibido no log da UI).
String mc_lastCmd();
