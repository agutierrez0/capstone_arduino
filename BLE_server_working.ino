#include <HCSR04.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

UltraSonicDistanceSensor ultrasonic(2,4);
int distance;

// left motor
int leftMotorSpeedPin = 14;
int leftMotorForwardPin = 27;
int leftMotorBackwardPin = 26;


// right motor
int rightMotorSpeedPin = 21;
int rightMotorForwardPin = 18;
int rightMotorBackwardPin = 19;

#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"
#define OTHER_CHARACTERISTIC_UUID "5d0faa6a-5086-11ec-bf63-0242ac130002"

int count = 0;
bool device_connected = false;
BLEServer *pServer;
BLEService *pService;
BLECharacteristic *pCharacteristic;
BLECharacteristic *otherCharacteristic;

class MyServerCallbacks: public BLEServerCallbacks {
  void onConnect(BLEServer* pServer) {
    device_connected = true;
  };

  void onDisconnect(BLEServer* pServer) {
    device_connected = false;
  };
};
void setup() {
    pinMode(leftMotorSpeedPin, OUTPUT);
    pinMode(leftMotorForwardPin, OUTPUT);
    pinMode(leftMotorBackwardPin, OUTPUT);
    pinMode(rightMotorSpeedPin, OUTPUT);
    pinMode(rightMotorForwardPin, OUTPUT);
    pinMode(rightMotorBackwardPin, OUTPUT);

    Serial.begin(9600);

  digitalWrite(leftMotorSpeedPin, HIGH);
  digitalWrite(rightMotorSpeedPin, HIGH);
  
  Serial.println("Starting BLE work!");

  BLEDevice::init("Long name works now");
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());
  pService = pServer->createService(SERVICE_UUID);
  pCharacteristic = pService->createCharacteristic(
                                         CHARACTERISTIC_UUID,
                                         BLECharacteristic::PROPERTY_READ |
                                         BLECharacteristic::PROPERTY_WRITE
                                       );

  pCharacteristic->setValue("Hello World says Mani");

  otherCharacteristic = pService->createCharacteristic(
                                         OTHER_CHARACTERISTIC_UUID,
                                         BLECharacteristic::PROPERTY_READ |
                                         BLECharacteristic::PROPERTY_WRITE
                                       );

  otherCharacteristic->setValue("Hello World says Angel");
  
  pService->start();
  // BLEAdvertising *pAdvertising = pServer->getAdvertising();  // this still is working for backward compatibility
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);  // functions that help with iPhone connections issue
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();
  Serial.println("Characteristic defined! Now you can read it in your phone!");
}

void loop() {
  distance = ultrasonic.measureDistanceCm(); //Use 'CM' for centimeters or 'INC' for inches
  Serial.println(distance);
  otherCharacteristic->setValue(distance);
  otherCharacteristic->notify();

  if (distance > -1 && distance < 15) {
    stop();
    pCharacteristic->setValue("stopping");
    delay(1000);
    goBackward();
    pCharacteristic->setValue("moving backwards");
    delay(300);
    stop();
    delay(1000);
    
    if(random(0, 2) == 0) {
      goLeft();
      pCharacteristic->setValue("moving left");
    } else {
      goRight();
      pCharacteristic->setValue("moving right");
    }
    
    delay(500);
    stop();
    pCharacteristic->setValue("stopping");
    delay(700);
  } else {
    goForward();
    pCharacteristic->setValue("moving forward");
  }
   
  pCharacteristic->notify();
  if (device_connected) {
    Serial.println("Device connected!");
  } else {
    Serial.println("Device not connected!");
  }
  
  Serial.println("Test");
 
  count = count + 1;
  delay(3000);
}

void goForward() {
  Serial.println("MOVING FORWARD");
  digitalWrite(leftMotorForwardPin, HIGH);
  digitalWrite(leftMotorBackwardPin, LOW);
  digitalWrite(rightMotorForwardPin, HIGH);
  digitalWrite(rightMotorBackwardPin, LOW);
}

void goBackward() {
  Serial.println("MOVING BACKWARDS");
  digitalWrite(leftMotorForwardPin, LOW);
  digitalWrite(leftMotorBackwardPin, HIGH);
  digitalWrite(rightMotorForwardPin, LOW);
  digitalWrite(rightMotorBackwardPin, HIGH);
}

void stop() {
  Serial.println("STOP");
  digitalWrite(leftMotorForwardPin, LOW);
  digitalWrite(leftMotorBackwardPin, LOW);
  digitalWrite(rightMotorForwardPin, LOW);
  digitalWrite(rightMotorBackwardPin, LOW);
}

void goRight() {
  Serial.println("RIGHT");
  digitalWrite(leftMotorForwardPin, HIGH);
  digitalWrite(leftMotorBackwardPin, LOW);
  digitalWrite(rightMotorForwardPin, LOW);
  digitalWrite(rightMotorBackwardPin, HIGH);
}

void goLeft() {
  Serial.println("LEFT");
  digitalWrite(leftMotorForwardPin, LOW);
  digitalWrite(leftMotorBackwardPin, HIGH);
  digitalWrite(rightMotorForwardPin, HIGH);
  digitalWrite(rightMotorBackwardPin, LOW);
}
