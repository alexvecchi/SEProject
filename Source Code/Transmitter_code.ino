#include <math.h>
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

int data[6];
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
  //delay(200); 
  //read left joystick values, note x and y are swaped; 
  xValue = analogRead(x_axis); 
  yValue = analogRead(y_axis);
  dValue = digitalRead(d_pin); 
  
  //read right joystick values, note x and y are swaped;  
  xValue_Servo = analogRead(x_axis_Servo); 
  yValue_Servo = analogRead(y_axis_Servo);
  dValue_Servo = digitalRead(d_pin_Servo); 
  
  //Send X,Y values
  data_map_send(xValue, yValue, dValue, xValue_Servo, yValue_Servo, dValue_Servo);
  
}

//maps and sends joysticks values 
void data_map_send(int xValue, int yValue, int dValue, int xValue_Servo, int yValue_Servo, int dValue_Servo){

  Serial.print("SX : ");
  Serial.print(yValue);
  Serial.print(" ");
  Serial.print(xValue);
  Serial.print(" ");
  Serial.print(dValue);
  Serial.print("   DX : ");
  Serial.print(xValue_Servo);
  Serial.print(" ");
  Serial.print(yValue_Servo);
  Serial.print(" ");
  Serial.println(dValue_Servo);  
  
  //Mapping X values Car JS
  if(xValue <= 470 || xValue >= 550){
    xValue = map(xValue, 0, 1023, -512, 512);
  }  
  else{
    xValue = 0;
  }
  data[0] = xValue;  
  
  //Mapping Y values Car JS
  if(yValue <= 470 || yValue >= 550){
    yValue = map(yValue, 0, 1023, -512, 512);
  } 
  else{
    yValue = 0;
  }
  data[1] = yValue;
  
  //Digital button value Car JS
  if(dValue == 0){
    data[2] = 100; 
  } 
  else{
    data[2] = 0;
  }
  
  //Mapping X values Tower JS
  if(xValue_Servo <= 470 || xValue_Servo >= 550){
    xValue_Servo = map(xValue_Servo, 0, 1023, -512, 512);
  }  
  else{
    xValue_Servo = 0;
  }
  data[3] = xValue_Servo;  
  
  //Mapping Y values Tower JS
  if(yValue_Servo <= 470 || yValue_Servo >= 550){
    yValue_Servo = map(yValue_Servo, 0, 1023, -512, 512);
  } 
  else{
    yValue_Servo = 0;
  }
  data[4] = yValue_Servo;
  
  //Digital button value Tower JS
  if(dValue_Servo == 0){
    data[5] = 200; 
  } 
  else{
    data[5] = 0;
  } 
  
  //Sending X,Y and D values
  radio.write(&data, sizeof(data));
  //Serial.println(servoMode);
  //delay(10); 
  
}
