
#include <HCSR04.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include "MPU9250.h"

int status;
UltraSonicDistanceSensor ultrasonic(2,4);

// defines variables
long duration;
int distance;

// BACK MOTORS
// left motor
int standby = 5;  // was leftMotorSpeedPin
int leftMotorForwardPin = 18;
int leftMotorBackwardPin = 19;

// right motor
int rightMotorSpeedPin = 23;
int rightMotorForwardPin = 33;
int rightMotorBackwardPin = 32;

// FRONT MOTORS
// left motor 
int LMotorSpeedPin = 13;
int LMotorForwardPin = 12;
int LMotorBackwardPin = 14;

// right motor
int RMotorSpeedPin = 25;
int RMotorForwardPin = 27;
int RMotorBackwardPin = 26;

#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"
#define OTHER_CHARACTERISTIC_UUID "5d0faa6a-5086-11ec-bf63-0242ac130002"
#define ANOTHER_CHARACTERISTIC_UUID "8fa2e117-e431-4dab-ab5a-24ba2067983d"
#define BEGIN_CHARACTERISTIC_UUID "e4085858-2331-4b31-af03-f485127f2e29"

bool device_connected = false;
BLEServer *pServer;
BLEService *pService;
BLECharacteristic *pCharacteristic;
BLECharacteristic *otherCharacteristic;
BLECharacteristic *anotherCharacteristic; 
BLECharacteristic *beginCharacteristic;

class MyServerCallbacks: public BLEServerCallbacks {
  void onConnect(BLEServer* pServer) {
    device_connected = true;
  };

  void onDisconnect(BLEServer* pServer) {
    device_connected = false;
  };
};

void setup() {
  Serial.begin(9600);
  
  //MOTOR DRIVERS SETUP FOR L298N & DRV8833
  pinMode(standby, OUTPUT);
  pinMode(leftMotorForwardPin, OUTPUT);
  pinMode(leftMotorBackwardPin, OUTPUT);
  pinMode(rightMotorSpeedPin, OUTPUT);
  pinMode(rightMotorForwardPin, OUTPUT);
  pinMode(rightMotorBackwardPin, OUTPUT);

  pinMode(LMotorSpeedPin, OUTPUT);
  pinMode(LMotorForwardPin, OUTPUT);
  pinMode(LMotorBackwardPin, OUTPUT);

  pinMode(RMotorSpeedPin, OUTPUT);
  pinMode(RMotorForwardPin, OUTPUT);
  pinMode(RMotorBackwardPin, OUTPUT);

  digitalWrite(LMotorSpeedPin, HIGH);
  digitalWrite(RMotorSpeedPin, HIGH);
  digitalWrite(standby, HIGH);
  digitalWrite(rightMotorSpeedPin, HIGH);


  BLEDevice::init("Long name works now");
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());
  pService = pServer->createService(SERVICE_UUID);


  // Value or Charater #1 for exporting to website 
  pCharacteristic = pService->createCharacteristic(
                                         CHARACTERISTIC_UUID,
                                         BLECharacteristic::PROPERTY_READ |
                                         BLECharacteristic::PROPERTY_WRITE
                                       );

  pCharacteristic->setValue("Hello World says Mani");

  
  // Value or Charater #2 for exporting to website 
  anotherCharacteristic = pService->createCharacteristic(
                                         ANOTHER_CHARACTERISTIC_UUID,
                                         BLECharacteristic::PROPERTY_READ |
                                         BLECharacteristic::PROPERTY_WRITE
                                       );
  anotherCharacteristic->setValue("Hello World says Pedro");


  // Value or Charater #3 for exporting to website 
  otherCharacteristic = pService->createCharacteristic(
                                         OTHER_CHARACTERISTIC_UUID,
                                         BLECharacteristic::PROPERTY_READ |
                                         BLECharacteristic::PROPERTY_WRITE
                                       );

  // Value or Charater #4 for exporting to website 
  beginCharacteristic = pService->createCharacteristic(
                                         BEGIN_CHARACTERISTIC_UUID,
                                         BLECharacteristic::PROPERTY_READ |
                                         BLECharacteristic::PROPERTY_WRITE
                                       );
  beginCharacteristic->setValue("123");
  
  pService->start();
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();
  Serial.println("Characteristic defined! Now you can read it in your phone!");
}

void loop() {
  std::basic_string<char> value = beginCharacteristic->getValue();
  String realValue = value.c_str();

  // Value is A = start, Value is B = stop
  if (realValue == "a") {
    Serial.println("Process stopped");
  }

  if (realValue == "b") {
    otherCharacteristic->getValue();
    distance = ultrasonic.measureDistanceCm(); //Use 'CM' for centimeters or 'INC' for inches
    distance = ultrasonic.measureDistanceCm(); //Use 'CM' for centimeters or 'INC' for inches
    Serial.println("Distance: ");
    Serial.println(distance);
    Serial.println(" cm ");
    otherCharacteristic->setValue(distance);
    otherCharacteristic->notify();

    if (distance > -1 && distance < 15){
      stop_now();
      pCharacteristic->setValue("stopping");
      delay(500);
      goBackward();
      pCharacteristic->setValue("moving backwards");
      delay(500);
      stop_now();
      delay(500);

      if (random(0, 2) == 0) {
        goLeft();
        pCharacteristic->setValue("moving left");
      } else {
        goRight();
        pCharacteristic->setValue("moving right");
      }

      delay(500);
      stop_now();
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
  }
}

void goForward() {
  Serial.println("MOVING FORWARD");
  digitalWrite(leftMotorForwardPin, HIGH);
  digitalWrite(leftMotorBackwardPin, LOW);
  digitalWrite(rightMotorForwardPin, HIGH);
  digitalWrite(rightMotorBackwardPin, LOW);

  digitalWrite(LMotorForwardPin,HIGH); 
  digitalWrite(LMotorBackwardPin,LOW);
  digitalWrite(RMotorForwardPin,HIGH); 
  digitalWrite(RMotorBackwardPin,LOW);
}

void goBackward() {
  Serial.println("MOVING BACKWARDS");
  digitalWrite(leftMotorForwardPin, LOW);
  digitalWrite(leftMotorBackwardPin, HIGH);
  digitalWrite(rightMotorForwardPin, LOW);
  digitalWrite(rightMotorBackwardPin, HIGH);

  digitalWrite(LMotorForwardPin,LOW); 
  digitalWrite(LMotorBackwardPin,HIGH);
  digitalWrite(RMotorForwardPin,LOW); 
  digitalWrite(RMotorBackwardPin,HIGH);
}

void stop_now() {
  Serial.println("STOP");
  digitalWrite(leftMotorForwardPin, LOW);
  digitalWrite(leftMotorBackwardPin, LOW);
  digitalWrite(rightMotorForwardPin, LOW);
  digitalWrite(rightMotorBackwardPin, LOW);

  digitalWrite(LMotorForwardPin,LOW); 
  digitalWrite(LMotorBackwardPin,LOW);
  digitalWrite(RMotorForwardPin,LOW); 
  digitalWrite(RMotorBackwardPin,LOW);
}

void goRight() {
  Serial.println("RIGHT");
  digitalWrite(leftMotorForwardPin, HIGH);
  digitalWrite(leftMotorBackwardPin, LOW);
  digitalWrite(rightMotorForwardPin, LOW);
  digitalWrite(rightMotorBackwardPin, HIGH);

  digitalWrite(LMotorForwardPin,HIGH); 
  digitalWrite(LMotorBackwardPin,LOW);
  digitalWrite(RMotorForwardPin,LOW); 
  digitalWrite(RMotorBackwardPin,HIGH);
}

void goLeft() {
  Serial.println("LEFT");
  digitalWrite(leftMotorForwardPin, LOW);
  digitalWrite(leftMotorBackwardPin, HIGH);
  digitalWrite(rightMotorForwardPin, HIGH);
  digitalWrite(rightMotorBackwardPin, LOW);

  digitalWrite(LMotorForwardPin,LOW); 
  digitalWrite(LMotorBackwardPin,HIGH);
  digitalWrite(RMotorForwardPin,HIGH); 
  digitalWrite(RMotorBackwardPin,LOW);
}
