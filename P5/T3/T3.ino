#include <Arduino_LSM9DS1.h>
#include "BBTimer.hpp"

BBTimer timerMuestreo(BB_TIMER3);
volatile bool flagMuestrear = false;

void callbackMuestreo() { flagMuestrear = true; }

void setup() {
  Serial.begin(9600);
  while (!Serial);
  IMU.begin();
  timerMuestreo.setupTimer(100000, callbackMuestreo); // 100ms
  timerMuestreo.timerStart();
}

void loop() {
  if (flagMuestrear) {
    flagMuestrear = false;
    float ax, ay, az, gx, gy, gz, mx, my, mz;
    
    if (IMU.accelerationAvailable() && IMU.gyroscopeAvailable() && IMU.magneticFieldAvailable()) {
      IMU.readAcceleration(ax, ay, az);
      IMU.readGyroscope(gx, gy, gz);
      IMU.readMagneticField(mx, my, mz);

      // Enviamos SOLO los números separados por punto y coma
      Serial.print(ax); Serial.print(";");
      Serial.print(ay); Serial.print(";");
      Serial.print(az); Serial.print(";");
      Serial.print(gx); Serial.print(";");
      Serial.print(gy); Serial.print(";");
      Serial.print(gz); Serial.print(";");
      Serial.print(mx); Serial.print(";");
      Serial.print(my); Serial.print(";");
      Serial.println(mz); 
    }
  }
}