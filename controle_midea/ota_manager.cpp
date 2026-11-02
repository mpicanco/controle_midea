/**
 * @file ota_manager.cpp
 * @brief Implementacao do upload de firmware por HTTP.
 */

#include "ota_manager.h"
#include "pages.h"
#include <Update.h>

// ------------------------------------------------------------
//  GET /ota : pagina de upload
// ------------------------------------------------------------
static void handleOtaPage(WebServer& server)
{
    server.sendHeader("Cache-Control", "no-store");
    server.send_P(200, "text/html", OTA_HTML);
}

// ------------------------------------------------------------
//  POST /otaupload : chunks do binario
// ------------------------------------------------------------
static void handleOtaChunk(WebServer& server)
{
    HTTPUpload& up = server.upload();

    if (up.status == UPLOAD_FILE_START) {
        Serial.printf("\n[OTA] inicio: %s\n", up.filename.c_str());
        // grava na particao OTA (app1), como fallback mantem a atual
        if (!Update.begin(UPDATE_SIZE_UNKNOWN)) {
            Update.printError(Serial);
        }
    } else if (up.status == UPLOAD_FILE_WRITE) {
        if (Update.write(up.buf, up.currentSize) != up.currentSize) {
            Update.printError(Serial);
        }
    } else if (up.status == UPLOAD_FILE_END) {
        if (Update.end(true)) {
            Serial.printf("[OTA] OK: %u bytes -> particao OTA\n", up.totalSize);
        } else {
            Update.printError(Serial);
        }
    } else if (up.status == UPLOAD_FILE_ABORTED) {
        Serial.println("[OTA] upload abortado!");
        Update.end();
    }
    yield();
}

// ------------------------------------------------------------
//  resposta final do POST (apos ultimo chunk)
// ------------------------------------------------------------
static void handleOtaReply(WebServer& server)
{
    server.sendHeader("Connection", "close");
    bool ok = !Update.hasError();
    server.send(ok ? 200 : 500, "text/html",
        "<html><body style='background:#02060d;color:#4dd8ff;font-family:monospace;"
        "text-align:center;padding-top:40vh;letter-spacing:2px'>"
        + (ok ? String("FIRMWARE RECEBIDO!<br>REINICIANDO...")
              : String("FALHA NO OTA!<br><a style='color:#4dd8ff' href='/ota'>tentar de novo</a>"))
        + "</body></html>");
    if (ok) {
        delay(800);
        ESP.restart();
    }
}

void ota_register(WebServer& server)
{
    server.on("/ota", HTTP_GET, [&server]() { handleOtaPage(server); });
    server.on("/otaupload", HTTP_POST,
              [&server]() { handleOtaReply(server); },
              [&server]() { handleOtaChunk(server); });
}
