#include <WiFi.h>
#include <ESPping.h> // Necesitarás instalar esta librería para el ping
#include "time.h"

const char* ssid = "DAVID";          // Nombre de tu red 
const char* password = "contraseña";  // Contraseña de tu red 
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 3600;      // UTC+1 (Invierno en España)
const int   daylightOffset_sec = 3600; // Desplazamiento horario de verano

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
}

void loop() {
  delay(1000);
  printLocalTime(); // Imprime la hora cada segundo en la terminal [cite: 23]
}


// Función para captar y mostrar la hora en la terminal 
void printLocalTime() {
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Error: No se pudo captar la hora del servidor.");
    return;
  }
  // Formato: Día de la semana, Mes Día Año Hora:Minuto:Segundo 
  Serial.print("Hora captada del server: ");
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
}