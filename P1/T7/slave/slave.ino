#include <Wire.h>
#include <Adafruit_NeoPixel.h>

#define PIN 48
Adafruit_NeoPixel pixels(1, PIN, NEO_GRB + NEO_KHZ800);

volatile bool dataReceived = false;
String receivedData = "";

void onReceive(int len) {
  receivedData = "";
  while (Wire.available()) {
    receivedData += (char)Wire.read();
  }
  dataReceived = true;
}

void setup() {
  Serial.begin(115200);
  Wire.begin(8); // Dirección del esclavo
  Wire.onReceive(onReceive);
  
  pixels.begin();
  pixels.setBrightness(50);
  Serial.println("Esperando datos I2C...");
}

void loop() {
  if (dataReceived) {
    Serial.print("Recibido: ");
    Serial.println(receivedData);
  
    pixels.setPixelColor(0, pixels.Color(0, 0, 255)); // Azul
    pixels.show();
    delay(1000); 
    pixels.setPixelColor(0, pixels.Color(0, 0, 0));
    pixels.show();
    
    dataReceived = false;
  }
}