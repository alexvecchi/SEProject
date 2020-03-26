/*
* Arduino Wireless Communication Tutorial
*     Example 1 - Transmitter Code
*                
* by Dejan Nedelkovski, www.HowToMechatronics.com
* 
* Library: TMRh20/RF24, https://github.com/tmrh20/RF24/
*/

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(9, 10); // CE, CSN

const byte address[6] = "00001";
const int x_axis = A1;
const int y_axis = A0; 
const int d_pin = 6;

const int x_axis_Servo = A2;
const int y_axis_Servo = A3;
const int d_pin_Servo = 5;
int servoMode = 0;

int data[5];
int xValue;
int yValue;
int dValue;
int xValue_Servo;
int yValue_Servo;
int dValue_Servo;

void setup() {
  pinMode(x_axis, INPUT);
  pinMode(y_axis, INPUT);
  pinMode(d_pin, INPUT); 
  pinMode(x_axis_Servo, INPUT);
  pinMode(y_axis_Servo, INPUT);
  pinMode(d_pin_Servo, INPUT); 
  Serial.begin(9600);
  digitalWrite(d_pin, HIGH);
  digitalWrite(d_pin_Servo, HIGH);
  radio.begin();
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_MIN);
  radio.stopListening();
  servoMode = 0;
}

void loop() {
  /*
  const char text[] = "Hello World";
  radio.write(&text, sizeof(text));
  delay(1000);
  Serial.println(text);
*/
  
  xValue = analogRead(x_axis); 
  yValue = analogRead(y_axis);
  dValue = digitalRead(d_pin); 
  Serial.print("X - ");
  Serial.print(xValue);
  Serial.print(" Y - ");
  Serial.print(yValue);
  Serial.print(" D - ");
  Serial.println(dValue);
  xValue_Servo = analogRead(x_axis_Servo); 
  yValue_Servo = analogRead(y_axis_Servo);
  dValue_Servo = digitalRead(d_pin_Servo); 
  Serial.print("X - ");
  Serial.print(xValue);
  Serial.print(" Y - ");
  Serial.print(yValue);
  Serial.print(" D - ");
  Serial.println(dValue);
  
  //Send X,Y values
  data_map_send(xValue, yValue, dValue, xValue_Servo, yValue_Servo, dValue_Servo);
  /*if(xValue == 5 && yValue == 15){
  data[0] = 0;
  radio.write(data, 1);
  Serial.println(data[0]);
  delay(500);
   }
  } 
 */
 /*
  }
  else{
  //Servo joystick 
  xValue_Servo = analogRead(x_axis_Servo);
  xValue_Servo = map(xValue_Servo, 0, 1023, 300, 400);
  if((xValue_Servo >= 300 && xValue_Servo <= 345) || (xValue_Servo >= 355 && xValue_Servo <= 400)) {
    //Send X-axis data of servo
    data[0] = xValue_Servo; 
    radio.write(data, 1); 
    Serial.println(data[0]);
    delay(500);
  }
  else{
  yValue_Servo = analogRead(y_axis_Servo);
  yValue_Servo = map(yValue_Servo, 0, 1023, 401, 501);
  if((yValue_Servo >= 401 && yValue_Servo <= 445) || (yValue_Servo >= 455 && yValue_Servo <= 501)) {
    //Send Y-axis data of servo
    data[0] = yValue_Servo; 
    radio.write(data, 1); 
    Serial.println(data[0]);
    delay(500);
    }
    else{
      data[0] = 0;
      radio.write(data, 1); 
      Serial.println(data[0]);
      delay(500);
    }
  }
  }
  
    
  dValue_Servo = digitalRead(d_pin_Servo); 
  if(dValue_Servo == 0){
    data[0] = 200;
    radio.write(data, 1);
    Serial.println(data[0]);
    delay(500);
  }
  //SERVO MODE ON-OFF
  dValue = digitalRead(d_pin); 
  if(dValue == 0){
    data[0] = 100;
    if(servoMode == 0){
      servoMode = 1;}
    else if(servoMode == 1){
      servoMode = 0;}
    radio.write(data, 1);
    Serial.println(data[0]);
    delay(500);
  }
      */
}

void data_map_send(int xValue, int yValue, int dValue, int xValue_Servo, int yValue_Servo, int dValue_Servo){
  //Mapping X values
  if(xValue <= 470){
    xValue = map(xValue, 470, 0, 1000, 1470);
  } 
  else if(xValue >= 550){
    xValue = map(xValue, 550, 1023, 1550, 2023);
  }
  else{
    xValue = 0;
  }
  data[0] = xValue;  
  
  //Mapping Y values
  if(yValue <= 470){
    yValue = map(yValue, 470, 0, 3000, 3470);
  }
    
  else if(yValue >= 550){
    yValue = map(yValue, 550, 1023, 3550, 4023);
  } 
  else{
    yValue = 0;
  }
  data[1] = yValue;
  
  //Digital button value
  if(dValue == 0){
    data[2] = 100;
    servoMode = !servoMode; 
  } 
  else{
    data[2] = 0;
  }
  
  if(xValue_Servo <= 470){
    xValue_Servo = map(xValue_Servo, 470, 0, 1000, 1470);
  } 
  else if(xValue_Servo >= 550){
    xValue_Servo = map(xValue_Servo, 550, 1023, 1550, 2023);
  }
  else{
    xValue_Servo = 0;
  }
  data[3] = xValue_Servo;
    
  //Digital button value
  if(dValue_Servo == 0){
    data[4] = 200;
  } 
  else{
    data[4] = 0;
  }
  //Sending X,Y and D values
  radio.write(&data, sizeof(data));
  //Serial.println(servoMode);
  //delay(10); 
  
}
