#include <Wire.h>
#include <Arduino_LSM9DS1.h>

float aX[5], gX[5], mX[5];

void setup() {
  Serial.begin(115200);
  Wire.begin(); // Inicia como Maestro
  IMU.begin();
  Serial.println("Envía 's' por consola para capturar y enviar.");
}

void loop() {
  if (Serial.read() == 's') {
    for(int i=0; i<5; i++) {
      if (IMU.accelerationAvailable()) IMU.readAcceleration(aX[i], aX[i], aX[i]);
      if (IMU.gyroscopeAvailable()) IMU.readGyroscope(gX[i], gX[i], gX[i]);
      if (IMU.magneticFieldAvailable()) IMU.readMagneticField(mX[i], mX[i], mX[i]);
      delay(200); 
    }
    
   
    for(int i=0; i<5; i++) {
      Wire.beginTransmission(8); //Dirección
      String data = "A:" + String(aX[i],1) + " G:" + String(gX[i],1) + " M:" + String(mX[i],1);
      Wire.print(data);
      Wire.endTransmission();
      delay(10); 
    }
    Serial.println("Datos enviados por I2C.");
  }
}
