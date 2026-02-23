#include <Arduino_LSM9DS1.h>
#include "BBTimer.hpp"

// Configuración de tiempos
const unsigned long INTERVALO_MUESTREO_US = 100000; // 100ms en microsegundos
BBTimer timerMuestreo(BB_TIMER3);
volatile bool flagMuestrear = false;

// Estructura para almacenar una muestra completa
struct DatosIMU {
  float ax, ay, az; // Acelerómetro
  float gx, gy, gz; // Giróscopo
  float mx, my, mz; // Magnetómetro
};

DatosIMU buffer[10]; // Buffer para 10 muestras (1 segundo de datos)
int indiceMuestra = 0;

void callbackMuestreo() {
  flagMuestrear = true;
}

void setup() {
  Serial.begin(9600);
  while (!Serial);

  // Inicialización del sensor LSM9DS1
  if (!IMU.begin()) {
    Serial.println("Error: No se pudo inicializar la IMU (LSM9DS1).");
    while (1);
  }

  Serial.println("IMU iniciada. Muestreo cada 100ms, reporte cada 1s.");

  // Configurar timer hardware para el muestreo
  timerMuestreo.setupTimer(INTERVALO_MUESTREO_US, callbackMuestreo);
  timerMuestreo.timerStart();
}

void loop() {
  if (flagMuestrear) {
    flagMuestrear = false;

    // Leer sensores si están disponibles
    if (IMU.accelerationAvailable() && IMU.gyroscopeAvailable() && IMU.magneticFieldAvailable()) {
      
      IMU.readAcceleration(buffer[indiceMuestra].ax, buffer[indiceMuestra].ay, buffer[indiceMuestra].az);
      IMU.readGyroscope(buffer[indiceMuestra].gx, buffer[indiceMuestra].gy, buffer[indiceMuestra].gz);
      IMU.readMagneticField(buffer[indiceMuestra].mx, buffer[indiceMuestra].my, buffer[indiceMuestra].mz);
      
      indiceMuestra++;

      // Al completar 10 muestras (1 segundo), enviamos por UART
      if (indiceMuestra >= 10) {
        Serial.println("--- Reporte Segundo IMU ---");
        for (int i = 0; i < 10; i++) {
          Serial.print("T+"); Serial.print(i * 100); Serial.print("ms | ");
          Serial.print("Acc(g): "); Serial.print(buffer[i].ax, 2); Serial.print(", "); Serial.print(buffer[i].ay, 2); Serial.print(", "); Serial.print(buffer[i].az, 2);
          Serial.print(" | Gyr(dps): "); Serial.print(buffer[i].gx, 1); Serial.print(", "); Serial.print(buffer[i].gy, 1);
          Serial.print(" | Mag(uT): "); Serial.println(buffer[i].mx, 1);
        }
        Serial.println();
        indiceMuestra = 0; // Reiniciar buffer
      }
    }
  }
}