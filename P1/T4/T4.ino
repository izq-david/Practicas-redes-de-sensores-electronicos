#include <Arduino.h>
#include "mbed.h"
#include "BBTimer.hpp"

using namespace mbed;

// Configuración de pines y periféricos
const int analogPin = A0;
PwmOut ledPwm(digitalPinToPinName(D3));
BBTimer timerUART(BB_TIMER2); // Usamos el Timer 2 para el envío periódico [cite: 34]

// Variables de control
volatile bool flagEnviarADC = false;
String inputString = "";
bool stringComplete = false;

// Callback del timer para envíos periódicos
void uartTimerCallback() {
    flagEnviarADC = true;
}

void setup() {
    Serial.begin(9600);
    inputString.reserve(50); // Reserva para evitar fragmentación de memoria [cite: 38, 203]
    
    ledPwm.period_us(200); // Frecuencia de 5kHz fija [cite: 29]
    
    Serial.println("Protocolo UART listo. Comandos: ADC, ADC(x), PWM(x)");
}

void loop() {
    // 1. Procesar entrada de consola (UART)
    while (Serial.available()) {
        char inChar = (char)Serial.read();
        if (inChar == '\n' || inChar == '\r') {
            if (inputString.length() > 0) stringComplete = true;
        } else {
            inputString += inChar;
        }
    }

    // 2. Ejecutar comandos recibidos 
    if (stringComplete) {
        inputString.trim();

        if (inputString == "ADC") {
            // Envío puntual de la lectura actual
            Serial.print("Lectura manual ADC: ");
            Serial.println(analogRead(analogPin));
        } 
        else if (inputString.startsWith("ADC(") && inputString.endsWith(")")) {
            // Envío periódico cada x segundos [cite: 33]
            int x = inputString.substring(4, inputString.length() - 1).toInt();
            if (x > 0) {
                timerUART.timerStop(); // Método correcto según tu clase 
                timerUART.setupTimer(x * 1000000, uartTimerCallback); // x en microsegundos 
                timerUART.timerStart();
                Serial.print("Envio periodico activado cada ");
                Serial.print(x);
                Serial.println("s.");
            } else {
                timerUART.timerStop(); // Si x=0, deja de mandar datos [cite: 33, 202]
                Serial.println("Envio periodico desactivado.");
            }
        } 
        else if (inputString.startsWith("PWM(") && inputString.endsWith(")")) {
            // Control del duty cycle (0 al 9) [cite: 35]
            int val = inputString.substring(4, inputString.length() - 1).toInt();
            if (val >= 0 && val <= 9) {
                float duty = val / 9.0f; // Mapeo de 0-9 a 0.0-1.0
                ledPwm.write(duty);
                Serial.print("PWM Duty Cycle ajustado al ");
                Serial.print(duty * 100);
                Serial.println("%.");
            }
        }

        inputString = "";
        stringComplete = false;
    }

    // 3. Gestión de la interrupción del Timer para envío periódico [cite: 34]
    if (flagEnviarADC) {
        flagEnviarADC = false;
        Serial.print("Lectura periodica ADC: ");
        Serial.println(analogRead(analogPin));
    }
}