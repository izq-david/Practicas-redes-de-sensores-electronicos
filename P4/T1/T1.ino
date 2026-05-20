#include <ArduinoBLE.h>

void setup() {
  if (!BLE.begin()) {
    while (1); // Se detiene si falla la inicialización
  }

  // Sustituye "TuNombre" por tu nombre real para el advertising
  BLE.setLocalName("David_BLE33"); 
  
  // Inicia el advertising
  BLE.advertise();
}

void loop() {
  // Atiende los eventos BLE
  BLE.poll(); 

}