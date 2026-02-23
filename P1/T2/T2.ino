#include <Arduino.h>
#include "BBTimer.hpp" // Asegúrate de tener el archivo en tu carpeta

const int analogPin = A0;
BBTimer timerADC(BB_TIMER1); // Usamos el Timer 1
volatile bool flagLeerADC = false;

// Función que se ejecuta cada 10 segundos
void adcCallback() {
    flagLeerADC = true;
}

void setup() {
    Serial.begin(9600);
    while (!Serial);

    // Configurar 10 segundos (10,000,000 microsegundos)
    timerADC.setupTimer(10000000, adcCallback);
    timerADC.timerStart();

    Serial.println("Timer Hardware configurado: Lectura cada 10s");
}

void loop() {
    if (flagLeerADC) {
        flagLeerADC = false;
        int lectura = analogRead(analogPin);
        Serial.print("Lectura ADC (10s): ");
        Serial.println(lectura);
    }
}