#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include "ServoTimer2.h"
#define trigPin A1
#define echoPin A0
#define lightSensor A2
#define selfDriveMode_led A3

RF24 radio(9, 10); // CE, CSN
float duration, distance;
const byte address[6] = "00001";
const int LeftMotorForward = A3;
const int LeftMotorBackward = A2;
const int RightMotorForward = 7;
const int RightMotorBackward = 8;
const int SPEED_LEFT = 5;
const int SPEED_RIGHT = 6;
const int trigger = 2;
ServoTimer2 servoA;  //Torretta
ServoTimer2 servoB;   //Sensore
int servoAAngle = 1500;
int servoBAngle = 1500;
int data[5];
int servoMode;
int activationSequence;
int speed_l, speed_r;
void setup() {

  speed_l = 0;
  speed_r = 0;
  pinMode(lightSensor, INPUT);
  activationSequence = 0;
  Serial.begin(9600);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  radio.begin();
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_MIN);
  pinMode(RightMotorForward, OUTPUT);
  pinMode(RightMotorBackward, OUTPUT);
  pinMode(LeftMotorForward, OUTPUT);
  pinMode(LeftMotorBackward, OUTPUT);
  pinMode(trigger, OUTPUT);
  pinMode(SPEED_LEFT, OUTPUT);
  pinMode(SPEED_RIGHT, OUTPUT);
  servoA.attach(4);
  servoA.write(servoAAngle);

  servoMode = 0;
  digitalWrite(RightMotorForward, LOW);
  digitalWrite(RightMotorBackward, LOW);
  digitalWrite(LeftMotorForward, LOW);
  digitalWrite(LeftMotorBackward, LOW);
  digitalWrite(trigger, LOW);

  pinMode(4, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(selfDriveMode_led, OUTPUT);
  radio.startListening();
}

void loop() {
  //int lightSensorValue = ReadLightSensorValue(lightSensor);
  if (radio.available()) {
    //Serial.print("Servo:");
    //Serial.println(servoMode);
    /*char text[32] = "";
      radio.read(&text, sizeof(text));*/
    radio.read(&data, sizeof(data));
    Serial.print("Data: X - ");
    Serial.print(data[0]);
    Serial.print(" Y - ");
    Serial.print(data[1]);
    Serial.print(" D - ");
    Serial.println(data[2]);
    car_move(data);
    tower_move(data);
    if (data[2] == 100) {
      selfDriveMode();
    }
  }
}
float readSensor() {

  // Write a pulse to the HC-SR04 Trigger Pin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Measure the response from the HC-SR04 Echo Pin
  duration = pulseIn(echoPin, HIGH);

  // Determine distance from duration
  // Use 343 metres per second as speed of sound

  distance = (duration / 2) * 0.0343;

  // Send results to Serial Monitor

  Serial.print("Distance = ");
  if (distance >= 400 || distance <= 2) {
    Serial.println("Out of range");
  }
  else {
    Serial.print(distance);
    Serial.println(" cm");
  }
  return distance;
}

void selfDriveMode() {

  servoB.attach(3);
  do {
    servoB.write(servoBAngle);
    radio.read(data, sizeof(data));
    float distance = readSensor();
    if (distance < 30) {
      digitalWrite(RightMotorForward, LOW);
      digitalWrite(RightMotorBackward, LOW);
      digitalWrite(LeftMotorForward, LOW);
      digitalWrite(LeftMotorBackward, LOW);

      int tempAngle;
      //Read left side
      for (tempAngle = servoBAngle; tempAngle < 2250; tempAngle += 10)
      {
        servoB.write(tempAngle);
        distance = readSensor();
        if (distance > 30) {
          break;
        }
      }
      //Read right side
      servoB.write(servoBAngle);
      for (tempAngle = servoBAngle; tempAngle > 750; tempAngle -= 10)
      {
        servoB.write(tempAngle);
        distance = readSensor();
        if (distance > 30) {
          break;
        }
      }
      if (distance > 30) {
        int angle = tempAngle - servoBAngle;
        if (angle > 0) {
          //left
          digitalWrite(RightMotorForward, HIGH);
          digitalWrite(RightMotorBackward, LOW);
          digitalWrite(LeftMotorForward, LOW);
          digitalWrite(LeftMotorBackward, HIGH);
          speed_l = 170;
          speed_r = 255;
          analogWrite(SPEED_LEFT, speed_l);
          analogWrite(SPEED_RIGHT, speed_r);
          Serial.print("LEFT - ");
          Serial.print("SR ");
          Serial.print(speed_r);
          Serial.print(" SL ");
          Serial.println(speed_l);
          delay(abs(angle) * 5);
        }
        else if (angle < 0) {
          digitalWrite(RightMotorForward, HIGH);
          digitalWrite(RightMotorBackward, LOW);
          digitalWrite(LeftMotorForward, LOW);
          digitalWrite(LeftMotorBackward, HIGH);
          speed_r = 170;
          speed_l = 255;
          analogWrite(SPEED_LEFT, speed_l);
          analogWrite(SPEED_RIGHT, speed_r);
          Serial.print("LEFT - ");
          Serial.print("SR ");
          Serial.print(speed_r);
          Serial.print(" SL ");
          Serial.println(speed_l);
          delay(abs(angle) * 5);
        }
      }
    }
    else {
      digitalWrite(RightMotorForward, HIGH);
      digitalWrite(RightMotorBackward, LOW);
      digitalWrite(LeftMotorForward, HIGH);
      digitalWrite(LeftMotorBackward, LOW);
      speed_l = speed_r = 255;
      analogWrite(SPEED_LEFT, speed_l);
      analogWrite(SPEED_RIGHT, speed_r);
      Serial.print("LEFT - ");
      Serial.print("SR ");
      Serial.print(speed_r);
      Serial.print(" SL ");
      Serial.println(speed_l);
    }
  } while (data[2] != 100);
}
void tower_move(int data[5]) {

  if (data[3] >= 1000 && data[3] <= 1470) {
    //Move servoA UP
    servoAAngle += 10;
    if (servoAAngle <= 2250) {
      servoA.write(servoAAngle);
    }
  }
  if (data[3] >= 1550 && data[3] <= 2023) {
    //Move servoA DOWN
    servoAAngle -= 10;
    if (servoAAngle >= 750) {
      servoA.write(servoAAngle);
    }
  }
  Serial.println(data[4]);
  if (data[4] == 200) {
    //pull the trigger, BANG
    Serial.print("BANG");
    digitalWrite(trigger, HIGH);
    delay(1000);
    digitalWrite(trigger, LOW);
  }
}

//Takes the input data, maps it, and uses it to move the car
void car_move(int data[5]) {
  int speed_l, speed_r;
  if (data[0] >= 1000 && data[0] <= 1470)
  {
    // STRAIGHT
    digitalWrite(RightMotorForward, HIGH);
    digitalWrite(RightMotorBackward, LOW);
    digitalWrite(LeftMotorForward, HIGH);
    digitalWrite(LeftMotorBackward, LOW);
    speed_r = speed_l = map(data[0], 1000, 1470, 170, 255);
    analogWrite(SPEED_LEFT, speed_l);
    analogWrite(SPEED_RIGHT, speed_r);
    Serial.print("STRAIGHT - ");
    Serial.print("SR ");
    Serial.print(speed_r);
    Serial.print(" SL ");
    Serial.println(speed_l);
  }
  else if (data[0] >= 1550 && data[0] <= 2023) {
    //BWD
    digitalWrite(RightMotorForward, LOW);
    digitalWrite(RightMotorBackward, HIGH);
    digitalWrite(LeftMotorForward, LOW);
    digitalWrite(LeftMotorBackward, HIGH);
    speed_r = speed_l = map(data[0], 1550, 2023, 170, 255);
    analogWrite(SPEED_LEFT, speed_l);
    analogWrite(SPEED_RIGHT, speed_r);
    Serial.print("BACK - ");
    Serial.print("SR ");
    Serial.print(speed_r);
    Serial.print(" SL ");
    Serial.println(speed_l);
  }

  else if (data[1] >= 3550 && data[1] <= 4023 && data[0] == 0) {
    // LEFT
    digitalWrite(RightMotorForward, HIGH);
    digitalWrite(RightMotorBackward, LOW);
    digitalWrite(LeftMotorForward, LOW);
    digitalWrite(LeftMotorBackward, HIGH);
    speed_l = map(data[0], 3550, 4023, 170, 255);
    speed_r = 255;
    analogWrite(SPEED_LEFT, speed_l);
    analogWrite(SPEED_RIGHT, speed_r);
    Serial.print("LEFT - ");
    Serial.print("SR ");
    Serial.print(speed_r);
    Serial.print(" SL ");
    Serial.println(speed_l);
  }
  else if (data[1] >= 3000 && data[1] <= 3470 && data[0] == 0) {
    // RIGHT
    digitalWrite(RightMotorForward, LOW);
    digitalWrite(RightMotorBackward, HIGH);
    digitalWrite(LeftMotorForward, HIGH);
    digitalWrite(LeftMotorBackward, LOW);
    speed_r = map(data[0], 3000, 3470, 170, 255);
    speed_l = 255;
    analogWrite(SPEED_LEFT, speed_l);
    analogWrite(SPEED_RIGHT, speed_r);
    Serial.print("RIGHT - ");
    Serial.print("SR ");
    Serial.print(speed_r);
    Serial.print(" SL ");
    Serial.println(speed_l);
  }
  else if (data[0] == 0 && data[1] == 0) {
    digitalWrite(RightMotorForward, LOW);
    digitalWrite(RightMotorBackward, LOW);
    digitalWrite(LeftMotorForward, LOW);
    digitalWrite(LeftMotorBackward, LOW);
  }
}

int ReadLightSensorValue(int pinNumber) {
  int lighSensorValue = analogRead(pinNumber);
  Serial.print("Light:");
  Serial.println(lighSensorValue);
  return lighSensorValue;
}
