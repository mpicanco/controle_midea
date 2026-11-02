/*
 * Controle de Ar-condicionado Midea via IR + WebServer (ESP32 WROOM DevKit V4)
 *
 * Baseado na biblioteca esp32_rmt_midea_ir_tx
 *
 * Projeto estruturado em modulos:
 *   config.h          -> pinos e constantes
 *   midea_control.*   -> estado + comandos do ar (IR)
 *   wifi_manager.*    -> WiFi persistente + portal de configuracao
 *   pages.h           -> paginas HTML (PROGMEM)
 *   web_server.*      -> rotas HTTP
 *
 * Hardware:
 *   - ESP32 DevKit V4 (WROOM-32)
 *   - LED emissor IR (940nm) no GPIO 4, com resistor ~100R no pino e
 *     transistor (ex.: 2N2222) se quiser mais alcance.
 *
 * WiFi (provisionamento):
 *   - Primeira vez: ESP32 abre AP "Midea_Remote" (rede aberta).
 *     Conecte o celular -> portal cativo abre a pagina de configuracao
 *     (ou http://192.168.4.1/setup).
 *   - Credenciais ficam salvas na flash (NVS); nos proximos boots
 *     conecta direto na rede da casa.
 *   - Segurar o botao BOOT 5s apaga credenciais e volta ao portal.
 */

#include "config.h"
#include "midea_control.h"
#include "wifi_manager.h"
#include "web_server.h"

void setup()
{
    Serial.begin(115200);
    Serial.println("\nBooting Midea Remote (controle_midea v1.0)...");

    mc_init();          // IR: RMT no GPIO MIDEA_TX_PIN
    wifi_begin();       // STA conectado OU portal de configuracao
    // (web_server_begin() e chamado internamente por wifi_begin)
}

void loop()
{
    wifi_loop();        // DNS do portal / reconexao / botao BOOT
    web_server_loop();  // processa clientes HTTP
    delay(2);
}
