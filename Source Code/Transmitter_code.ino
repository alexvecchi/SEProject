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
  pinMode(d_pin, INPUT); //or INPUT_PULLUP
  pinMode(x_axis_Servo, INPUT); 
  pinMode(y_axis_Servo, INPUT);
  pinMode(d_pin_Servo, INPUT); //or INPUT_PULLUP
  Serial.begin(9600); //set datarate in bps (baude) per serial data transmission
  digitalWrite(d_pin, HIGH);
  digitalWrite(d_pin_Servo, HIGH);
  radio.begin();
  radio.openWritingPipe(address); //open a new pipe to communicate via byte array
  radio.setPALevel(RF24_PA_MIN); //set Power Amplifier level to -18dbm
  radio.stopListening(); //switch to transmitt mode
  servoMode = 0;
}

void loop() {
  
  //read left joystick values, note x and y are swaped; 
  xValue = analogRead(x_axis); 
  yValue = analogRead(y_axis);
  dValue = digitalRead(d_pin); 
  
  //read right joystick values, note x and y are swaped;  
  xValue_Servo = analogRead(x_axis_Servo); 
  yValue_Servo = analogRead(y_axis_Servo);
  dValue_Servo = digitalRead(d_pin_Servo); 
  
//maps and sends joysticks values 
  
  //Mapping X values Car JS
  if(xValue <= 470 || xValue >= 550){
    xValue = map(xValue, 0, 1023, -512, 512);
  }  
  else{
    xValue = 0;
  }
  data[0] = xValue; //insert into the array
  
  //Mapping Y values Car JS
  if(yValue <= 470 || yValue >= 550){
    yValue = map(yValue, 0, 1023, -512, 512);
  } 
  else{
    yValue = 0;
  }
  data[1] = yValue; //insert into the array
  
  //Digital button value Car JS
  if(dValue == 0){
    data[2] = 100; //insert into the array
  } 
  else{
    data[2] = 0; //insert into the array
  }
  
  //Mapping X values Tower JS
  if(xValue_Servo <= 470 || xValue_Servo >= 550){
    xValue_Servo = map(xValue_Servo, 0, 1023, -512, 512);
  }  
  else{
    xValue_Servo = 0;
  }
  data[3] = xValue_Servo; //insert into the array
  
  //Mapping Y values Tower JS
  if(yValue_Servo <= 470 || yValue_Servo >= 550){
    yValue_Servo = map(yValue_Servo, 0, 1023, -512, 512);
  } 
  else{
    yValue_Servo = 0;
  }
  data[4] = yValue_Servo; //insert into the array
  
  //Digital button value Tower JS
  if(dValue_Servo == 0){
    data[5] = 200; //insert into the array
  } 
  else{
    data[5] = 0; //insert into the array
  } 
  
  //Sending X,Y and D values
  radio.write(&data, sizeof(data)); //transmitt to the adrress setted on the opening of the pipe, return an ACK

}
