#include <ArduinoBLE.h>
#include <Arduino_LSM9DS1.h>

BLEService imuService("181A"); 

BLEStringCharacteristic accelChar("2A5D", BLERead | BLENotify, 20); 

BLEByteCharacteristic enableChar("2A5E", BLEWrite); 

bool accelEnabled = true;

void setup() {
  BLE.begin();
  IMU.begin();

  BLE.setLocalName("David_BLE33"); 
  BLE.setAdvertisedService(imuService);

  imuService.addCharacteristic(accelChar);
  imuService.addCharacteristic(enableChar);
  BLE.addService(imuService);

  enableChar.writeValue(1);
  BLE.advertise();
}

void loop() {
  BLEDevice central = BLE.central();
  if (central) {
    while (central.connected()) {
     
      if (enableChar.written()) {
        accelEnabled = (enableChar.value() != 0);
      }

      
      if (accelEnabled && IMU.accelerationAvailable()) {
        float x, y, z;
        IMU.readAcceleration(x, y, z);
        String data = String(x, 1) + "," + String(y, 1) + "," + String(z, 1);
        accelChar.writeValue(data);
        delay(200); 
      }
    }
  }
}