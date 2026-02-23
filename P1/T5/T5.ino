#include <Wire.h>

void setup() {
  Wire.begin(); // Inicializa como Maestro
  Serial.begin(9600);
  while (!Serial);
  Serial.println("Maestro (Nano 33 BLE) listo. Envia '1' (ON) o '0' (OFF)");
}

void loop() {
  if (Serial.available()) {
    char comando = Serial.read();
    if (comando == '1' || comando == '0') {
      Wire.beginTransmission(8); // Dirección del ESP32
      Wire.write(comando);
      Wire.endTransmission();
      Serial.print("Comando I2C enviado: ");
      Serial.println(comando);
    }
  }
}