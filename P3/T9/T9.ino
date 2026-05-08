#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>      // Para SenML 
#include <ESP32_FTPClient.h>  // Para subir el archivo 
#include "time.h"

// Configuración de red y servidores
const char* ssid = "DAVID";
const char* password = "contraseña";
const char* ntpServer = "pool.ntp.org";
char ftp_server[] = "155.210.153.41"; // IP del laboratorio 
char ftp_user[]   = "rsense";         // Usuario 
char ftp_pass[]   = "renimer430";         // Password 

// Objetos y variables globales
WebServer server(80);
ESP32_FTPClient ftp(ftp_server, ftp_user, ftp_pass); // 
long offset_segundos = 0;
unsigned long lastFTPMillis = 0;

// --- Funciones para el Servidor Web (Tarea 3) ---

String getHTML() {
  time_t now;
  time(&now);
  time_t fake_now = now - offset_segundos;
  struct tm *fake_time = localtime(&fake_now);
  char time_str[20];
  strftime(time_str, sizeof(time_str), "%H:%M:%S", fake_time);

  String html = "<html><head><meta http-equiv='refresh' content='1'></head><body>";
  html += "<h1>Servidor Web ESP32</h1>";
  html += "<p>Hora actual: <strong>" + String(time_str) + "</strong></p>";
  html += "<form action='/reset' method='POST'><button type='submit'>Resetear Hora</button></form>";
  html += "</body></html>";
  return html;
}

void handleRoot() { server.send(200, "text/html", getHTML()); }

void handleReset() {
  time_t now;
  time(&now);
  offset_segundos = now;
  server.sendHeader("Location", "/");
  server.send(303);
}

// --- Funciones para SenML y FTP (Tarea 4) ---

String generarSenML() {
  StaticJsonDocument<256> doc; // 
  time_t now;
  time(&now);

  doc["bn"] = "esp32/David/"; // Base Name
  JsonArray e = doc.createNestedArray("e");
  
  JsonObject sensor = e.createNestedObject();
  sensor["n"] = "temp";    // Name 
  sensor["u"] = "cel";     // Unit (Celsius) 
  sensor["v"] = 22.0 + (rand() % 50) / 10.0; // Valor inventado 
  sensor["t"] = now;       // Marca temporal 

  String output;
  serializeJson(doc, output);
  return output;
}

void subirArchivoFTP() {
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)) return;

  // Formato de nombre: grupoXX_ddmmss.json [cite: 104]
  char fileName[32];
  strftime(fileName, sizeof(fileName), "grupoXX_%d%H%M%S.json", &timeinfo);

  String contenido = generarSenML();

  Serial.print("Subiendo a FTP: "); Serial.println(fileName);
  
  ftp.OpenConnection(); // 
  ftp.InitFile("Type I");
  ftp.ChangeWorkDir("/"); 
  ftp.NewFile(fileName);
  ftp.Write(contenido.c_str());
  ftp.CloseFile();
  ftp.CloseConnection();
}

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) { delay(500); Serial.print("."); }
  
  configTime(3600, 3600, ntpServer); // Sincronizar hora 

  server.on("/", handleRoot);
  server.on("/reset", HTTP_POST, handleReset);
  server.begin();
  Serial.println("\nSistemas listos.");
}

void loop() {
  server.handleClient(); // Atender navegador 

  // Tarea 4: Subida cada 10 segundos 
  if (millis() - lastFTPMillis > 10000) {
    subirArchivoFTP();
    lastFTPMillis = millis();
  }
}