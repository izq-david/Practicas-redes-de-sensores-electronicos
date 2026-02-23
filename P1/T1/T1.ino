// Definición del pin analógico a utilizar
const int analogPin = A0; 

void setup() {
  // Inicialización de la comunicación serie a 9600 bps
  Serial.begin(9600);
}

void loop() {
  // Lectura del valor del ADC (0 a 1023)
  int adcValue = analogRead(analogPin);
  
  // Mostrar el valor por consola
  Serial.print("Valor ADC: ");
  Serial.println(adcValue);
  
  // Esperar 1 segundo (1000 ms) antes de la siguiente lectura
  delay(1000); 
}