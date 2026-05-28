#include <WiFi.h>

const char* ssid = "DAVID";
const char* password = "cacacaca";
const char* host = "192.168.43.152"; 
const uint16_t port = 8080;

WiFiClient client;

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
}

void loop() {
  if (!client.connected()) {
    client.connect(host, port);
    delay(1000);
    return;
  }
  
  float ax = random(-50, 50) / 10.0;
  float ay = random(-50, 50) / 10.0;
  float az = random(-50, 50) / 10.0;
  
  client.println(String(ax) + ";" + String(ay) + ";" + String(az));
  delay(100);
}