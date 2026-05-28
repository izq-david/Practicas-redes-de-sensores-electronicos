#include "EspMQTTClient.h"


EspMQTTClient client(
  "DAVID",           // SSID 
  "cacacaca",       // Contraseña 
  "io.adafruit.com",   // Servidor MQTT de Adafruit
  "Perico1234",        // Usuario 
  "",        // AIO Key
  "ESP32_Genaro",      
  1883                 
);

void setup() {
  Serial.begin(115200);
}


void onConnectionEstablished() {
  Serial.println("¡Conectado a Adafruit IO por MQTT!");

  client.subscribe("Perico1234/feeds/aceleracion", [](const String & payload) {
    Serial.print("¡Nuevo dato detectado en la web!: ");
    Serial.println(payload);
  });

  client.publish("Perico1234/feeds/aceleracion", "9.81");
}

void loop() {
  client.loop(); 
}