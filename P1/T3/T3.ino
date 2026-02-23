#include <Arduino.h>
#include "mbed.h"
#include "BBTimer.hpp"

// Usamos el espacio de nombres de mbed para el PWM
using namespace mbed;

const int analogPin = A0;
const PinName pwmPin = digitalPinToPinName(D3); // Convertimos el pin de Arduino a PinName de Mbed

// Creamos el objeto PWM
PwmOut ledPwm(pwmPin);

BBTimer timerADC(BB_TIMER1);
volatile bool flagLeerADC = false;

void adcCallback() {
    flagLeerADC = true;
}

void setup() {
    Serial.begin(9600);
    while (!Serial);

    // Configurar el periodo del PWM: 1 / 5000Hz = 0.0002 segundos (200 microsegundos)
    ledPwm.period_us(200); 

    // Inicializar timer de la Tarea 2
    timerADC.setupTimer(10000000, adcCallback);
    timerADC.timerStart();

    Serial.println("Tarea 3: PWM a 5kHz (Periodo: 200us) configurado.");
}

void loop() {
    if (flagLeerADC) {
        flagLeerADC = false;
        
        // Leer ADC (0 a 1023)
        int valorADC = analogRead(analogPin);
        
        // Mbed utiliza valores de 0.0 a 1.0 para el Duty Cycle
        float dutyCycle = (float)valorADC / 1023.0f;
        ledPwm.write(dutyCycle);
        
        // Verificación por consola
        Serial.print("ADC: ");
        Serial.print(valorADC);
        Serial.print(" | Duty Cycle: ");
        Serial.print(dutyCycle * 100);
        Serial.println("%");
    }
}