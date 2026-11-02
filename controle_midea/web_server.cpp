/**
 * @file web_server.cpp
 * @brief Implementacao das rotas HTTP.
 *
 * Rotas:
 *   GET  /              -> painel HUD (ou portal se primeira configuracao)
 *   GET  /setup         -> pagina de configuracao WiFi
 *   POST /save          -> salva SSID/senha na NVS e reinicia
 *   GET  /scan          -> JSON com redes proximas (SSI, RSSI)
 *   GET  /status        -> JSON com estado do ar
 *   GET  /cmd?act=..&v= -> aplica comando e devolve JSON de estado
 */

#include "web_server.h"
#include "pages.h"
#include "midea_control.h"
#include "wifi_manager.h"
#include "ota_manager.h"
#include "config.h"
#include <WebServer.h>
#include <WiFi.h>

static WebServer server(80);

static void sendNoStore(const String& body, const char* type = "application/json")
{
    server.sendHeader("Cache-Control", "no-store");
    server.send(200, type, body);
}

static void sendPage(const char* page)
{
    server.sendHeader("Cache-Control", "no-store");
    server.send_P(200, "text/html", page);
}

// GET /
static void handleRoot()
{
    // na primeira conexao (sem rede salva) o "/" abre a configuracao
    if (wifi_isPortal() && !wifi_hasCredentials()) {
        sendPage(SETUP_HTML);
    } else {
        sendPage(INDEX_HTML);
    }
}

// GET /setup
static void handleSetup()
{
    sendPage(SETUP_HTML);
}

// GET /status
static void handleStatus()
{
    sendNoStore(mc_stateJson());
}

// GET /cmd?act=power|up|down|mode|osc|fan&v=0..3
static void handleCmd()
{
    String act = server.arg("act");
    Serial.println("[HTTP] /cmd act=" + act + " v=" + server.arg("v"));

    bool ok = mc_command(act, server.arg("v").toInt());
    if (ok) {
        sendNoStore(mc_stateJson());
    } else {
        server.send(400, "text/plain", "comando invalido");
    }
}

// GET /scan
static void handleScan()
{
    int n = WiFi.scanComplete();
    if (n == WIFI_SCAN_FAILED) {
        WiFi.scanNetworks(true);         // assincrono; proxima chamada traz resultado
        server.send(200, "application/json", "[]");
        return;
    }
    String j = "[";
    for (int i = 0; i < n; i++) {
        if (i) j += ",";
        String ssid = WiFi.SSID(i);
        ssid.replace("\\", "\\\\");
        ssid.replace("\"", "\\\"");
        j += "{\"ssid\":\"" + ssid + "\",\"rssi\":" + String(WiFi.RSSI(i)) + "}";
    }
    j += "]";
    sendNoStore(j);
}

// POST /save
static void handleSave()
{
    String ssid = server.arg("ssid");
    String pass = server.arg("pass");

    if (ssid.length() == 0) {
        server.send(400, "text/plain", "SSID vazio");
        return;
    }
    sendPage(SAVE_OK);
    wifi_saveCredentials(ssid, pass);
    Serial.println("WiFi salvo: " + ssid + " -> reiniciando");
    delay(800);
    ESP.restart();
}

// portal cativo: URL desconhecida -> redireciona para /setup
static void handleNotFound()
{
    if (wifi_isPortal()) {
        server.sendHeader("Location", String("http://") + WiFi.softAPIP().toString() + "/setup");
        server.send(302, "text/plain", "");
    } else {
        server.send(404, "text/plain", "not found");
    }
}

void web_server_begin()
{
    server.on("/", HTTP_GET, handleRoot);
    server.on("/setup", HTTP_GET, handleSetup);
    server.on("/status", HTTP_GET, handleStatus);
    server.on("/cmd", HTTP_GET, handleCmd);
    server.on("/scan", HTTP_GET, handleScan);
    server.on("/save", HTTP_POST, handleSave);
    ota_register(server);
    server.onNotFound(handleNotFound);
    server.begin();
    Serial.println("WebServer iniciado");
}

void web_server_loop()
{
    server.handleClient();
}
