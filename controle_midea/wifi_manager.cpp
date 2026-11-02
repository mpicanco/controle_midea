#include "wifi_manager.h"
#include "config.h"
#include "web_server.h"
#include <WiFi.h>
#include <DNSServer.h>
#include <ESPmDNS.h>
#include <Preferences.h>

static Preferences m_prefs;
static DNSServer   m_dns;

static bool     m_portal  = false;
static uint32_t m_btnHold = 0;

// ============================================================
//  Helpers NVS
// ============================================================
static String savedSsid()
{
    m_prefs.begin(NVS_NAMESPACE, true);
    String s = m_prefs.getString(NVS_KEY_SSID, "");
    m_prefs.end();
    return s;
}

static String savedPass()
{
    m_prefs.begin(NVS_NAMESPACE, true);
    String s = m_prefs.getString(NVS_KEY_PASS, "");
    m_prefs.end();
    return s;
}

void wifi_saveCredentials(const String& ssid, const String& pass)
{
    m_prefs.begin(NVS_NAMESPACE, false);
    m_prefs.putString(NVS_KEY_SSID, ssid);
    m_prefs.putString(NVS_KEY_PASS, pass);
    m_prefs.end();
}

static void clearCredentials()
{
    m_prefs.begin(NVS_NAMESPACE, false);
    m_prefs.clear();
    m_prefs.end();
}

// ============================================================
//  MODO PORTAL (AP de configuracao)
// ============================================================
static void portal_begin()
{
    m_portal = true;
    WiFi.mode(WIFI_AP);
    WiFi.softAP(AP_SSID, AP_PASS);           // AP_PASS vazio = rede aberta
    m_dns.start(53, "*", WiFi.softAPIP());

    // alias mDNS tambem na rede do AP (quando o cliente suportar)
    if (MDNS.begin(DEVICE_HOSTNAME)) {
        MDNS.addService("http", "tcp", 80);
    }

    Serial.println("\n=== MODO CONFIGURACAO ===");
    Serial.print("AP: "); Serial.println(AP_SSID);
    Serial.print("IP: "); Serial.println(WiFi.softAPIP());
    Serial.print("Alias: http://"); Serial.print(DEVICE_HOSTNAME); Serial.println(".local");
    Serial.println("Conecte o celular nesta rede; o portal abre sozinho");
    Serial.println("(ou http://192.168.4.1/setup)");

    web_server_begin();
}

// ============================================================
//  API publica
// ============================================================
void wifi_begin()
{
    String ssid = savedSsid();
    String pass = savedPass();

    if (ssid.length()) {
        WiFi.mode(WIFI_STA);
        WiFi.setHostname(DEVICE_HOSTNAME);   // nome visivel no roteador
        WiFi.begin(ssid.c_str(), pass.c_str());
        Serial.print("Conectando em \"" + ssid + "\"");
        uint32_t t0 = millis();
        while (WiFi.status() != WL_CONNECTED && millis() - t0 < STA_TIMEOUT_MS) {
            delay(500);
            Serial.print(".");
        }
        if (WiFi.status() == WL_CONNECTED) {
            Serial.println();
            Serial.print("Conectado! IP: ");
            Serial.println(WiFi.localIP());

            // alias mDNS: http://air_office.local
            if (MDNS.begin(DEVICE_HOSTNAME)) {
                MDNS.addService("http", "tcp", 80);
                Serial.print("Alias: http://");
                Serial.print(DEVICE_HOSTNAME);
                Serial.println(".local");
            }
            m_portal = false;
            web_server_begin();              // modo normal
            return;
        }
        Serial.println("\nFalha ao conectar -> abrindo portal");
    } else {
        Serial.println("Sem rede salva -> abrindo portal");
    }
    portal_begin();
}

void wifi_loop()
{
    // botao BOOT (GPIO0): segurar RESET_BTN_MS apaga as credenciais salvas
    if (digitalRead(RESET_BTN_PIN) == LOW) {
        if (m_btnHold == 0) {
            m_btnHold = millis();
        } else if (millis() - m_btnHold > RESET_BTN_MS) {
            clearCredentials();
            Serial.println("\nCredenciais apagadas! Voltando ao modo configuracao...");
            delay(1000);
            ESP.restart();
        }
    } else {
        m_btnHold = 0;
    }

    if (m_portal) {
        m_dns.processNextRequest();          // portal cativo
    } else if (WiFi.status() != WL_CONNECTED) {
        static uint32_t lastTry = 0;
        if (millis() - lastTry > WIFI_RECONNECT_MS) {
            lastTry = millis();
            WiFi.reconnect();
            Serial.println("WiFi caiu, reconectando...");
        }
    }
}

bool wifi_isPortal()
{
    return m_portal;
}

bool wifi_hasCredentials()
{
    return savedSsid().length() > 0;
}

String wifi_statusJson()
{
    String j = "{\"portal\":";
    j += m_portal ? "true" : "false";
    if (m_portal) {
        j += ",\"ap\":\"" AP_SSID "\",\"ip\":\"";
        j += WiFi.softAPIP().toString();
    } else {
        j += ",\"ssid\":\"";
        j += WiFi.SSID();
        j += "\",\"ip\":\"";
        j += WiFi.localIP().toString();
    }
    j += "\",\"rssi\":";
    j += String(WiFi.RSSI());
    j += "}";
    return j;
}
