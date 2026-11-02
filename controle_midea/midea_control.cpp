#include "midea_control.h"
#include "config.h"
#include <midea_ir.h>
#include <Preferences.h>

static MideaIR m_ir;
static String  m_lastCmd = "BOOT";
static bool    m_osc     = false;   // palhetas oscilando (toggle estimado)

// ============================================================
//  Persistencia do ajuste (NVS)
// ============================================================
// Grava/le o conjunto completo do ajuste: on, modo, temperatura e
// ventilacao. Ao LIGAR o estado salvo e restaurado antes de enviar
// o sinal IR (enabled forcada para true para nao desfazer o toggle),
// garantindo que o AC receba sempre o ultimo ajuste.
static void stateSave()
{
    Preferences p;
    p.begin(NVS_STATE_NAMESPACE, false);
    p.putBool("on",   m_ir.enabled);
    p.putUChar("mode", (uint8_t)m_ir.mode);
    p.putUChar("fan", m_ir.fan_level);
    p.putUChar("temp", m_ir.temperature);
    p.putBool("osc",  m_osc);
    p.end();
}

static void stateLoad()
{
    Preferences p;
    p.begin(NVS_STATE_NAMESPACE, true);
    m_ir.enabled       = p.getBool("on", false);
    uint8_t mode       = p.getUChar("mode", DEFAULT_MODE);
    m_ir.fan_level     = p.getUChar("fan", DEFAULT_FAN_LEVEL);
    m_ir.temperature   = p.getUChar("temp", DEFAULT_TEMP);
    m_osc              = p.getBool("osc", false);
    p.end();

    // valida valores: so aceita frio/quente; qualquer coisa vira frio
    if (mode != (uint8_t)MODE_COOL && mode != (uint8_t)MODE_HEAT) {
        mode = DEFAULT_MODE;
    }
    m_ir.mode = (MideaMode)mode;

    if (m_ir.temperature < TEMP_MIN) m_ir.temperature = TEMP_MIN;
    if (m_ir.temperature > TEMP_MAX) m_ir.temperature = TEMP_MAX;
}

void mc_init()
{
    midea_ir_init(&m_ir, MIDEA_RMT_CHANNEL, MIDEA_TX_PIN);
    m_lastCmd = "BOOT";

    stateLoad();
    stateSave();    // garante registro inicial na NVS
}

static void sendIRState(const String& label)
{
    midea_ir_send(&m_ir);
    stateSave();    // ajuste persistente a cada comando
    m_lastCmd = label;
}

bool mc_command(const String& act, int v)
{
    if (act == "power") {
        m_ir.enabled = !m_ir.enabled;
        if (m_ir.enabled) {
            // restaura o ultimo ajuste salvo (temp/modo/vent) e mantem LIGADO;
            // stateLoad() traz enabled=false gravado, forcar reativar depois
            stateLoad();
            m_ir.enabled = true;
        }
        sendIRState(m_ir.enabled ? "POWER ON" : "POWER OFF");
        return true;
    }
    if (act == "up") {
        if (m_ir.temperature < TEMP_MAX) m_ir.temperature++;
        sendIRState("TEMP+" + String(m_ir.temperature));
        return true;
    }
    if (act == "down") {
        if (m_ir.temperature > TEMP_MIN) m_ir.temperature--;
        sendIRState("TEMP-" + String(m_ir.temperature));
        return true;
    }
    if (act == "mode") {
        m_ir.mode = (m_ir.mode == MODE_COOL) ? MODE_HEAT : MODE_COOL;
        sendIRState(m_ir.mode == MODE_COOL ? "MODO FRIO" : "MODO QUENTE");
        return true;
    }
    if (act == "osc") {
        m_osc = !m_osc;             // estado do toggle (estimado; sem feedback IR)
        midea_ir_oscilate();
        m_lastCmd = m_osc ? "PALHETAS ON" : "PALHETAS OFF";
        return true;
    }
    if (act == "fan") {
        if (v < 0 || v > 3) return false;
        m_ir.fan_level = (uint8_t)v;
        sendIRState(String("VENT. ") + (v == 0 ? "AUTO" : String(v)));
        return true;
    }
    return false;
}

String mc_stateJson()
{
    String j = "{\"on\":";
    j += m_ir.enabled ? "true" : "false";
    j += ",\"mode\":\"";
    j += (m_ir.mode == MODE_HEAT) ? "heat" : "cool";
    j += "\",\"temp\":";
    j += String(m_ir.temperature);
    j += ",\"fan\":";
    j += String(m_ir.fan_level);
    j += ",\"osc\":";
    j += m_osc ? "true" : "false";
    j += ",\"last\":\"";
    j += m_lastCmd;
    j += "\"}";
    return j;
}

String mc_lastCmd()
{
    return m_lastCmd;
}
