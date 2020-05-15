#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Servo.h>
#define trigPin A1
#define echoPin A0

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
double deg = 0;
Servo servoA;  //Tower
Servo servoB;   //Sensor
int servoAAngle = 90;
int servoBAngle = 90;
int data[6];
int servoMode;
int activationSequence;
int speed_l, speed_r; 
int mapped_x = 0;
int mapped_y = 0; 
double rad = 0; 

void setup() {
  speed_l = 255;
  speed_r = 255; 
  pinMode(lightSensor, INPUT);
  activationSequence = 0;
  Serial.begin(9600); //set datarate in bps (baude) per serial data transmission
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  radio.begin();
  radio.openReadingPipe(0, address); //open a new pipe to communicate via byte array
  radio.setPALevel(RF24_PA_MIN); //set Power Amplifier level to -18dbm
  pinMode(RightMotorForward, OUTPUT);
  pinMode(RightMotorBackward, OUTPUT);
  pinMode(LeftMotorForward, OUTPUT);
  pinMode(LeftMotorBackward, OUTPUT);
  pinMode(trigger, OUTPUT);
  pinMode(SPEED_LEFT, OUTPUT);
  pinMode(SPEED_RIGHT, OUTPUT);
  pinMode(lightPin, OUTPUT);
  servoA.attach(4);
  servoA.write(servoAAngle); //define the servo angle
  servoB.attach(3);
  servoB.write(servoBAngle); //define the servo angle

  servoMode = 0;
  digitalWrite(RightMotorForward, LOW);
  digitalWrite(RightMotorBackward, LOW);
  digitalWrite(LeftMotorForward, LOW);
  digitalWrite(LeftMotorBackward, LOW);
  digitalWrite(trigger, LOW);
  digitalWrite(lightPin, LOW);
  pinMode(4, OUTPUT);
  pinMode(3, OUTPUT);
  radio.startListening(); //switch to receive mode
}

void loop() {
  //check the light sensor 
  int lighSensorValue = analogRead(lightSensor); 
  if(lighSensorValue > 600)
    digitalWrite(lightPin, HIGH);
  else
    digitalWrite(lightPin, LOW);

 //check if data are available to be read
 if (radio.available()) {
  radio.read(&data, sizeof(data));
    
 //car_move(data)
  mapped_x = 0;
  mapped_y = 0;
  //Radians calculation
  rad = atan2 (data[0], data[1]); // In radians
  //Degrees calculation
  deg = rad * 180 / PI;
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
  
//tower_move(data)

  mapped_x = 0;
  mapped_y = 0;
  //Radians calculation
  rad = atan2 (data[3], data[4]); // In radians
  //Degrees calculation
  deg = rad * 180 / PI;
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
    delay(80); //move the motor to throw
    digitalWrite(trigger, LOW);
  }

//check if the selfdrive move is enabled
 if (data[2] == 100) {
  do {
    float distance = readSensor(); //read the sensor to calculate the distance
    
      if (distance < 30) {
      //stop the car
      digitalWrite(RightMotorForward, LOW);
      digitalWrite(RightMotorBackward, LOW);
      digitalWrite(LeftMotorForward, LOW);
      digitalWrite(LeftMotorBackward, LOW);
      delay(5);
      //TURN RIGHT AND GET DISTANCE
      Serial.println("---------RIGHT");
      servoB.write(0); //turn the servo of the sensor to right
      delay(1000); //time to positioning the servo
      float distanceRight = readSensor(); //take the distance to right
      delay(5);
      //TURN LEFT AND GET DISTANCE
      servoB.write(180); //turn the servo of the sensor to left
      delay(1000); //time to positioning the servo
      float distanceLeft = readSensor(); //take the distance to left
      delay(5);
      
      if (distanceRight > distanceLeft) {
        //turn the car right
        digitalWrite(RightMotorForward, LOW);
        digitalWrite(RightMotorBackward, HIGH);
        digitalWrite(LeftMotorForward, HIGH);
        digitalWrite(LeftMotorBackward, LOW);
        analogWrite(SPEED_LEFT, 80);
        analogWrite(SPEED_RIGHT, 80);
        delay(1000);
      }
      else {
        //turn the car left
        digitalWrite(RightMotorForward, HIGH);
        digitalWrite(RightMotorBackward, LOW);
        digitalWrite(LeftMotorForward, LOW);
        digitalWrite(LeftMotorBackward, HIGH);
        analogWrite(SPEED_LEFT, 80);
        analogWrite(SPEED_RIGHT, 80);
        delay(1000);
      }
      servoB.write(90); //reset position of the servo of the sensor
    }
    //go straight
    Serial.println("--------- FW ");
    digitalWrite(RightMotorForward, HIGH);
    digitalWrite(RightMotorBackward, LOW);
    digitalWrite(LeftMotorForward, HIGH);
    digitalWrite(LeftMotorBackward, LOW);
    analogWrite(SPEED_LEFT, 80);
    analogWrite(SPEED_RIGHT, 80);
    radio.read(data, sizeof(data));
  } while (data[2] != 100); //if the selfdrive is pressed again the selfdrive mode is disabled
    } //end if
  } //end if
} //end loop
 
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
