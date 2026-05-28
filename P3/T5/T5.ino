#include <WiFi.h>
#include "time.h"
#include <ESPping.h> 

// Datos del servidor (tu PC)
const char* host = "10.191.150.152"; 
const uint16_t port = 1234;        

const char* ssid = "WifiPortatilAGR";          // Nombre de la red 
const char* password = "12345678";  // Contraseña de la red 
const char* ntpServer1 = "time.google.com";
const char* ntpServer2 = "pool.ntp.org";
const long  gmtOffset_sec = 3600;      // UTC+1 (Invierno en España)
const int   daylightOffset_sec = 3600; // Desplazamiento horario de verano

WiFiClient client;
bool transmissionActive = false;

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
  
  // Le pasamos los dos servidores
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer1, ntpServer2);
  Serial.print("Sincronizando hora NTP");
  

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
      client.println("Transmision iniciada.");
    } else if (command == "stop") {
      transmissionActive = false;
      client.println("Transmision detenida.");
    }
  }

  // Enviar hora si la transmisión está activa (ESP32 -> PC)
    if (transmissionActive && client.connected()) {
      struct tm timeinfo;
      if (getLocalTime(&timeinfo)) {
        // Enviamos la hora formateada al PC 
        client.printf("Hora local ESP32: %02d:%02d:%02d\n", 
                      timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
      } else {
        // Si falla, avisamos por el socket y por el puerto serie
        client.println("Error: Hora no sincronizada con NTP todavía.");
        Serial.println("Fallo interno: No se pudo obtener la hora NTP.");
      }
      delay(1000); // Frecuencia de envío: 1 segundo 
    }
}