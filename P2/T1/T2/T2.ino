#include <Adafruit_NeoPixel.h>

#define PIN 48
#define NUMPIXELS 1

Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

// Estructura para los datos del sensor inercial
struct IMUData {
  float x, y, z;
};

// Cola de FreeRTOS para pasar datos entre tareas
QueueHandle_t queueIMU;

void TareaSensor(void *pvParameters) {
  IMUData datos;
  while (1) {
    // Simulamos datos aleatorios del acelerómetro
    datos.x = random(-2000, 2000) / 1000.0;
    datos.y = random(-2000, 2000) / 1000.0;
    datos.z = random(-2000, 2000) / 1000.0;
    
    // Enviamos a la cola
    xQueueSend(queueIMU, &datos, portMAX_DELAY);
    
    vTaskDelay(100 / portTICK_PERIOD_MS); // Muestrea cada 100 ms 
  }
}

void TareaUART_LED(void *pvParameters) {
  IMUData datosRecibidos;
  while (1) {
    // 1. Extraer e imprimir todos los datos acumulados por UART
    Serial.println("--- Nuevos datos (1 segundo) ---");
    while (uxQueueMessagesWaiting(queueIMU) > 0) {
      xQueueReceive(queueIMU, &datosRecibidos, 0);
      Serial.printf("Acel -> X: %.2f g, Y: %.2f g, Z: %.2f g\n", datosRecibidos.x, datosRecibidos.y, datosRecibidos.z);
    }

    // 2. Encender LED 
    pixels.setPixelColor(0, pixels.Color(0, 0, 255)); // Azul
    pixels.show();
    
    // 3. Mantener encendido 200 ms 
    vTaskDelay(200 / portTICK_PERIOD_MS); 
    
    // 4. Apagar LED
    pixels.setPixelColor(0, pixels.Color(0, 0, 0)); 
    pixels.show();
    
    // 5. Esperar los 800 ms restantes para completar el ciclo de 1 segundo 
    vTaskDelay(800 / portTICK_PERIOD_MS); 
  }
}

void setup() {
  Serial.begin(115200);
  pixels.begin();
  pixels.setBrightness(50);
  
  // Crear cola con capacidad para 10 muestras (1 segundo a 100ms/muestra)
  queueIMU = xQueueCreate(10, sizeof(IMUData));
  
  // Creación de las tareas RTOS
  xTaskCreate(TareaSensor, "Sensor", 2048, NULL, 1, NULL);
  xTaskCreate(TareaUART_LED, "UART_LED", 4096, NULL, 1, NULL);
}

void loop() {
  // En FreeRTOS el loop se queda vacío
}