#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Servo.h>
#define trigPin A1
#define echoPin A0
#define selfDriveMode_led A3

RF24 radio(9, 10); // CE, CSN
float duration;
int distance;
int prevMove;
const byte address[6] = "00001";
const int LeftMotorForward = A3;
const int LeftMotorBackward = A2;
const int RightMotorForward = 7;
const int RightMotorBackward = 8;
const int SPEED_LEFT = 5;
const int SPEED_RIGHT = 6;
const int trigger = A4;
const int lightSensor = A5;
const int lightPin = 2;
Servo servoA;  //Torretta
Servo servoB;   //Sensor
int servoAAngle = 90;
int servoBAngle = 90;
int data[6];
int servoMode;
int activationSequence;
int speed_l, speed_r;
void setup() {
  speed_l = 255;
  speed_r = 255;
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
  pinMode(lightPin, OUTPUT);
  servoA.attach(4);
  servoA.write(servoAAngle);
  servoB.attach(3);
  servoB.write(servoBAngle);

  servoMode = 0;
  digitalWrite(RightMotorForward, LOW);
  digitalWrite(RightMotorBackward, LOW);
  digitalWrite(LeftMotorForward, LOW);
  digitalWrite(LeftMotorBackward, LOW);
  digitalWrite(trigger, LOW);
  digitalWrite(lightPin, LOW);
  pinMode(4, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(selfDriveMode_led, OUTPUT);
  radio.startListening();
}

void loop() {
  //delay(200);
  int lighSensorValue = analogRead(lightSensor); 
  if(lighSensorValue > 600)
    digitalWrite(lightPin, HIGH);
  else
    digitalWrite(lightPin, LOW);
 
  if (radio.available()) {
    //Serial.print("Servo:");
    //Serial.println(servoMode);
    /*char text[32] = "";
      radio.read(&text, sizeof(text));*/
    radio.read(&data, sizeof(data));
    /*
      Serial.print("Car: ");
      Serial.print(data[0]);
      Serial.print(" °");
      Serial.print(" D - ");
      Serial.println(data[1]);
      Serial.print("Tower: ");
      Serial.print(data[2]);
      Serial.print(" °");
      Serial.print(" D - ");
      Serial.println(data[3]);  */
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

  distance = duration * 0.034 / 2;

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
  do {
    float distance = readSensor();
    if (distance < 30) {
      //prevMove = random(0,2);
      //STOP THE CAR
      digitalWrite(RightMotorForward, LOW);
      digitalWrite(RightMotorBackward, LOW);
      digitalWrite(LeftMotorForward, LOW);
      digitalWrite(LeftMotorBackward, LOW);
      delay(5);
      //TURN RIGHT AND GET DISTANCE
      Serial.println("---------RIGHT");
      /*digitalWrite(RightMotorForward, LOW);
        digitalWrite(RightMotorBackward, HIGH);
        digitalWrite(LeftMotorForward, HIGH);
        digitalWrite(LeftMotorBackward, LOW);
        analogWrite(SPEED_LEFT, 80);
        analogWrite(SPEED_RIGHT, 80);
        delay(1000); */
      servoB.write(0);
      delay(1000);
      float distanceRight = readSensor();
      delay(5);
      //TURN LEFT AND GET DISTANCE
      /*Serial.println("---------LEFT");
        digitalWrite(RightMotorForward, HIGH);
        digitalWrite(RightMotorBackward, LOW);
        digitalWrite(LeftMotorForward, LOW);
        digitalWrite(LeftMotorBackward, HIGH);
        analogWrite(SPEED_LEFT, 80);
        analogWrite(SPEED_RIGHT, 80);
        delay(2000);  */
      servoB.write(180);
      delay(1000);
      float distanceLeft = readSensor();
      delay(5);
      if (distanceRight > distanceLeft) {
        digitalWrite(RightMotorForward, LOW);
        digitalWrite(RightMotorBackward, HIGH);
        digitalWrite(LeftMotorForward, HIGH);
        digitalWrite(LeftMotorBackward, LOW);
        analogWrite(SPEED_LEFT, 80);
        analogWrite(SPEED_RIGHT, 80);
        delay(1000);
      }
      else {
        digitalWrite(RightMotorForward, HIGH);
        digitalWrite(RightMotorBackward, LOW);
        digitalWrite(LeftMotorForward, LOW);
        digitalWrite(LeftMotorBackward, HIGH);
        analogWrite(SPEED_LEFT, 80);
        analogWrite(SPEED_RIGHT, 80);
        delay(1000);
      }
      servoB.write(90);
    }
    Serial.println("--------- FW ");
    digitalWrite(RightMotorForward, HIGH);
    digitalWrite(RightMotorBackward, LOW);
    digitalWrite(LeftMotorForward, HIGH);
    digitalWrite(LeftMotorBackward, LOW);
    analogWrite(SPEED_LEFT, 80);
    analogWrite(SPEED_RIGHT, 80);
    radio.read(data, sizeof(data));
  } while (data[2] != 100);
}
void tower_move(int data[6]) {


  int mapped_x = 0;
  int mapped_y = 0;
  //Radians
  double rad = atan2 (data[3], data[4]); // In radians
  //Degrees
  double deg = rad * 180 / PI;
  Serial.print("Tower ° ");
  Serial.println(deg);

  if (deg <= 110 && deg >= 60) {
    //Move servoA ; UP
    servoAAngle += 1;
    if (servoAAngle <= 180) {
      servoA.write(servoAAngle);
    }
  }
  if (deg >= -135 && deg <= -60) {
    //Move servoA; DOWN
    servoAAngle -= 1;
    if (servoAAngle >= 0) {
      servoA.write(servoAAngle);
    }
  }

  Serial.println(data[5]);
  if (data[5] == 200) {
    //pull the trigger, BANG
    Serial.print("BANG");
    digitalWrite(trigger, HIGH);
    delay(80);
    digitalWrite(trigger, LOW);
  }
}

//Takes the input data, maps it, and uses it to move the car
void car_move(int data[6]) {
  int mapped_x = 0;
  int mapped_y = 0;
  //Radians
  double rad = atan2 (data[0], data[1]); // In radians
  //Degrees
  double deg = rad * 180 / PI;
  // not used for the moment
  //double dist = sqrt(mapped_x*mapped_x + mapped_y*mapped_y);
  Serial.print("Car ° ");
  Serial.println(deg);
  if (deg >= -130 && deg <= -50) {
    Serial.println("FW");
    digitalWrite(RightMotorForward, HIGH);
    digitalWrite(RightMotorBackward, LOW);
    digitalWrite(LeftMotorForward, HIGH);
    digitalWrite(LeftMotorBackward, LOW);
    analogWrite(SPEED_LEFT, 255);
    analogWrite(SPEED_RIGHT, 255);
  }
  if (deg >= 60 && deg <= 130) {
    Serial.println("BW");
    digitalWrite(RightMotorForward, LOW);
    digitalWrite(RightMotorBackward, HIGH);
    digitalWrite(LeftMotorForward, LOW);
    digitalWrite(LeftMotorBackward, HIGH);
    analogWrite(SPEED_LEFT, 255);
    analogWrite(SPEED_RIGHT, 255);
  }
  if (deg >= 140 || deg <= -150) {
    Serial.println("RT");
    digitalWrite(RightMotorForward, LOW);
    digitalWrite(RightMotorBackward, HIGH);
    digitalWrite(LeftMotorForward, HIGH);
    digitalWrite(LeftMotorBackward, LOW);
    analogWrite(SPEED_LEFT, 255);
    analogWrite(SPEED_RIGHT, 255);
  }
  if (deg >= -40 && deg <= 40 && data[1] != 0) {
    Serial.println("LT");
    digitalWrite(RightMotorForward, HIGH);
    digitalWrite(RightMotorBackward, LOW);
    digitalWrite(LeftMotorForward, LOW);
    digitalWrite(LeftMotorBackward, HIGH);
    analogWrite(SPEED_LEFT, 255);
    analogWrite(SPEED_RIGHT, 255);
  }
  if (data[1] == 0 && data[0] == 0) {
    digitalWrite(RightMotorForward, LOW);
    digitalWrite(RightMotorBackward, LOW);
    digitalWrite(LeftMotorForward, LOW);
    digitalWrite(LeftMotorBackward, LOW);
  }
}
