#include <WiFi.h>
#include <WebServer.h>
#include "time.h"
#include <ESPping.h> // Necesitarás instalar esta librería para el ping

WebServer server(80); // Servidor en el puerto estándar HTTP

const char* ssid = "DAVID";          // Nombre de tu red 
const char* password = "contraseña";  // Contraseña de tu red 
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 3600;      // UTC+1 (Invierno en España)
const int   daylightOffset_sec = 3600; // Desplazamiento horario de verano

// Variables para el control de la hora
struct tm timeinfo;
bool manual_reset = false;

// HTML de la página web
String getHTML() {
  char time_str[20];
  if (manual_reset) {
    sprintf(time_str, "00:00:00 (Reset)");
  } else if (getLocalTime(&timeinfo)) {
    strftime(time_str, sizeof(time_str), "%H:%M:%S", &timeinfo);
  } else {
    sprintf(time_str, "Sincronizando...");
  }

  String html = "<html><body><h1>Servidor Web ESP32</h1>";
  html += "<p>Hora actual: <strong>" + String(time_str) + "</strong></p>";
  html += "<form action='/reset' method='POST'><button type='submit'>Resetear Hora</button></form>";
  html += "</body></html>";
  return html;
}

void handleRoot() {
  server.send(200, "text/html", getHTML());
}

void handleReset() {
  manual_reset = true; 
  Serial.println("Hora reseteada manualmente desde la web.");
  server.sendHeader("Location", "/"); // Redirigir a la página principal
  server.send(303);
}

void setup() {
  Serial.begin(115200);

  // 1. Conexión a la red WiFi
  WiFi.begin(ssid, password);
  Serial.print("Conectando a WiFi");
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  // 2. Extraer y mostrar la IP 
  Serial.println("\nConexión establecida");
  Serial.print("IP asignada: ");
  Serial.println(WiFi.localIP());

  // 3. Comprobar conectividad con Google mediante un ping 
  Serial.println("Realizando ping a Google...");
  bool success = Ping.ping("www.google.com", 3);
  
  if (success) {
    Serial.println("¡Ping exitoso! Tienes acceso a internet.");
  } else {
    Serial.println("Fallo en el ping. Revisa la configuración de red.");
  }
  
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  Serial.println("Sincronizando hora...");

  server.on("/", handleRoot);      // Ruta principal
  server.on("/reset", HTTP_POST, handleReset); // Ruta para el botón
  server.begin();
  Serial.println("Servidor HTTP iniciado.");
}

void loop() {
  server.handleClient(); // Atender peticiones del navegador
}