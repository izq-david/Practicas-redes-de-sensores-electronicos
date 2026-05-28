#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

BLEServer *pServer = NULL;
BLECharacteristic *pTxCharacteristic;
bool deviceConnected = false;

// UUIDs del servicio estándar Nordic UART
#define SERVICE_UUID           "6E400001-B5A3-F393-E0A9-E50E24DCCA9E" 
#define CHARACTERISTIC_UUID_RX "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID_TX "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"

class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) { deviceConnected = true; }
    void onDisconnect(BLEServer* pServer) { 
      deviceConnected = false; 
      pServer->getAdvertising()->start(); // Reinicia el advertising
    }
};

class MyCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      String rxValue = pCharacteristic->getValue().c_str();
      if (rxValue.length() > 0) {
        Serial.print("Móvil: ");
        Serial.println(rxValue);
      }
    }
};

void setup() {
  Serial.begin(115200);
  BLEDevice::init("Chat_ESP32_S3");
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  BLEService *pService = pServer->createService(SERVICE_UUID);
  
  // TX (Transmisión del ESP32 al Móvil) - Notificaciones
  pTxCharacteristic = pService->createCharacteristic(CHARACTERISTIC_UUID_TX, BLECharacteristic::PROPERTY_NOTIFY);
  pTxCharacteristic->addDescriptor(new BLE2902());

  // RX (Recepción del Móvil al ESP32) - Escritura
  BLECharacteristic *pRxCharacteristic = pService->createCharacteristic(CHARACTERISTIC_UUID_RX, BLECharacteristic::PROPERTY_WRITE);
  pRxCharacteristic->setCallbacks(new MyCallbacks());

  pService->start();
  pServer->getAdvertising()->start();
  Serial.println("Esperando conexión BLE...");
}

void loop() {
  // Del PC al Móvil
  if (deviceConnected && Serial.available()) {
    String txValue = Serial.readStringUntil('\n');
    txValue.trim(); // Limpia saltos de línea
    if (txValue.length() > 0) {
      pTxCharacteristic->setValue(txValue.c_str());
      pTxCharacteristic->notify();
      Serial.print("PC: ");
      Serial.println(txValue);
    }
  }
}