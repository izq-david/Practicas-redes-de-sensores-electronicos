#include <Adafruit_NeoPixel.h>

#define PIN 48
#define NUMPIXELS 1

Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

void TareaBlink(void *pvParameters) {
  pixels.begin();
  pixels.setBrightness(50); 
  
  bool estado = false;
  
  while (1) {
    estado = !estado; 
    
    if (estado) {
      pixels.setPixelColor(0, pixels.Color(0, 255, 0)); // Verde
    } else {
      pixels.setPixelColor(0, pixels.Color(0, 0, 0));   // Apagado
    }
    pixels.show();
    
    
    vTaskDelay(200 / portTICK_PERIOD_MS); 
  }
}


void TareaUART(void *pvParameters) {
  while (1) {
    Serial.println("hola mundo");
    vTaskDelay(1000 / portTICK_PERIOD_MS); 
  }
}

void setup() {
  Serial.begin(115200);
  pixels.begin();
  pixels.setBrightness(50);
  
  xTaskCreate(TareaBlink, "Blink", 2048, NULL, 1, NULL);
  xTaskCreate(TareaUART, "UART", 1024, NULL, 1, NULL);
}

void loop() {
}