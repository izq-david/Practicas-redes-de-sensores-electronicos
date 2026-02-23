#include <Wire.h>

// El pinout indica que el LED incorporado está en el GPIO 38
const int ledPin = 38; 

void setup() {
  pinMode(ledPin, OUTPUT);
  // SDA = GPIO 8, SCL = GPIO 9 según el diagrama proporcionado
  Wire.begin(8, 8, 9); 
  Wire.onReceive(receiveEvent);
}

void loop() {
  delay(10);
}

void receiveEvent(int howMany) {
  while (Wire.available()) {
    char c = Wire.read();
    if (c == '1') digitalWrite(ledPin, HIGH);
    else if (c == '0') digitalWrite(ledPin, LOW);
  }
}