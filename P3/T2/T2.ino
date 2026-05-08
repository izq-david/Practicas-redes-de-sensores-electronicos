#include <WiFi.h>
#include "time.h"

// Datos del servidor (tu PC)
const char* host = "192.168.1.107"; // <-- CAMBIA ESTO por la IP de tu PC
const uint16_t port = 1234;        // El puerto que pusiste en SocketTest

WiFiClient client;
bool transmissionActive = false;

void setup() {
  // ... (Mantén tu código anterior de WiFi y NTP) ...
}

void loop() {
  // Intentar conectar si se pierde la conexión
  if (!client.connected()) {
    if (client.connect(host, port)) {
      Serial.println("Conectado al servidor SocketTest");
    }
  }

  // Leer comandos desde el PC (SocketTest -> ESP32)
  if (client.available()) {
    String command = client.readStringUntil('\n');
    command.trim(); // Limpiar espacios o saltos de línea

    if (command == "start") {
      transmissionActive = true;
      client.println("Transmisión iniciada.");
    } else if (command == "stop") {
      transmissionActive = false;
      client.println("Transmisión detenida.");
    }
  }

  // Enviar hora si la transmisión está activa (ESP32 -> PC)
  if (transmissionActive && client.connected()) {
    struct tm timeinfo;
    if (getLocalTime(&timeinfo)) {
      // Enviamos la hora formateada al PC 
      client.printf("Hora local ESP32: %02d:%02d:%02d\n", 
                    timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
    }
    delay(1000); // Frecuencia de envío: 1 segundo 
  }
}