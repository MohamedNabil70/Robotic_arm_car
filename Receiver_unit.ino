#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <RF24_config.h>
#include <Servo.h>





//Analog
#define LEFT_THRESHOLD  400
#define RIGHT_THRESHOLD 800
#define UP_THRESHOLD    400
#define DOWN_THRESHOLD  800

#define COMMAND_NO     0x00
#define COMMAND_LEFT   0x01
#define COMMAND_RIGHT  0x02
#define COMMAND_UP     0x04
#define COMMAND_DOWN   0x08

Servo servo_gripper;
Servo servo_side1;
Servo servo_side2;
Servo servo_base;

void Car_Forward (void);
void Car_Backward (void);
void Car_Right (void);
void Car_Left (void);
void Car_Stop(void);

int xValue = 0 ; // To store value of the X axis
int yValue = 0 ; // To store value of the Y axis
int command = COMMAND_NO;

 int X_pi;
 int Y_pi;

// Arduino pin numbers
const int SW_pin = 8; // digital pin connected to switch output
const int X_pin = 0; // analog pin connected to X output
const int Y_pin = 1; // analog pin connected to Y output

// Motor A connections
int enA = 2;
int in1 = A0;
int in2 = A1;
// Motor B connections
int enB = 4 ;
int in3 = A2;
int in4 = A3;



RF24 radio(7, 8); // CSN, CE

const uint64_t pipeIn = 0xF9E8F0F0E1LL; //IMPORTANT: The same as in the receiver 0xF9E8F0F0E1LL
unsigned long lastRecvTime = 0;

struct mostaqbel
{
int analog_x;
int analog_y;
int Pot_1;
int Pot_2;
int Pot_3;
int Pot_4;
};                                  
struct mostaqbel Data_in;

int servo_gripper_pin = 3;
int servo_side1_pin = 5;
int servo_side2_pin = 10;
int servo_base_pin = 9;


//Assign default input received values
void setInputDefaultValues()
{
  // The middle position for joystick. (254/2=127)
Data_in.analog_x=127;
Data_in.analog_y=127;

Data_in.Pot_1=0;
Data_in.Pot_2=0;
Data_in.Pot_3=0;
Data_in.Pot_4=0;
}

void mapAndWriteValues()
{
  int s1,s2,s3,s4;
  s1= map(Data_in.Pot_2,0,1023, 0,180);
  s2= map(Data_in.Pot_1, 0,1023, 0,180);
  s3= map(Data_in.Pot_3, 0,1023, 0,180);
  s4= map(Data_in.Pot_4, 0,1023, 0,180);
  servo_gripper.write(s1);
  servo_side1.write(s2);
  servo_side2.write(s3);
  servo_base.write(s4);
  
}

void mapAndWriteCar()
{
   Car_Stop();
  // read analog X and Y analog values
  xValue = Data_in.analog_x;
  yValue = Data_in.analog_y;

  // converts the analog value to commands
  // reset commands
  command = COMMAND_NO;

  // check left/right commands
  if (xValue < LEFT_THRESHOLD)
    command = command | COMMAND_LEFT;
  else if (xValue > RIGHT_THRESHOLD)
    command = command | COMMAND_RIGHT;

  // check up/down commands
  if (yValue < UP_THRESHOLD)
    command = command | COMMAND_UP;
  else if (yValue > DOWN_THRESHOLD)
    command = command | COMMAND_DOWN;

  // NOTE: AT A TIME, THERE MAY BE NO COMMAND, ONE COMMAND OR TWO COMMANDS

  // print command to serial and process command
  if (command & COMMAND_LEFT) {
    Serial.println("COMMAND LEFT");
    Car_Left();
    // TODO: add your task here
  }

  if (command & COMMAND_RIGHT) {
    Serial.println("COMMAND RIGHT");
    Car_Right();
    // TODO: add your task here
  }

  if (command & COMMAND_UP) {
    Serial.println("COMMAND UP");
    Car_Forward();
    // TODO: add your task here
  }

  if (command & COMMAND_DOWN) {
    Serial.println("COMMAND DOWN");
    Car_Backward();
    // TODO: add your task here
  }  

 delay(10);

  
}

void setup() {
  Serial.begin(9600);
  radio.begin();
  radio.setDataRate(RF24_250KBPS);
  radio.openReadingPipe(1,pipeIn);

 // radio.setPALevel(RF24_PA_MIN);
  radio.startListening();
  servo_gripper.attach (servo_gripper_pin) ; 
  servo_side1.attach (servo_side1_pin) ; 
  servo_side2.attach (servo_side2_pin) ; 
  servo_base.attach (servo_base_pin) ; 

 // Set all the motor control pins to outputs
  pinMode(enA, OUTPUT);
  pinMode(enB, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);
  
  analogWrite(enA, 255);
  analogWrite(enB, 255);
  
  setInputDefaultValues();
  mapAndWriteValues();
  mapAndWriteCar();
}

void loop() {
  if (radio.available()) {

  radio.read(&Data_in,sizeof(mostaqbel)); 
    lastRecvTime = millis(); 
       char inputValuesString[100];
    sprintf(inputValuesString, 
            "%3d,%3d,%3d,%3d,%3d,%3d",
            Data_in.analog_x,
            Data_in.analog_y,
            Data_in.Pot_1,
            Data_in.Pot_2,
            Data_in.Pot_3,
            Data_in.Pot_4);
    Serial.println(inputValuesString); 
  }
   mapAndWriteValues();      
   mapAndWriteCar();   
}


void directionControl() {
  // Set motors to maximum speed
  // For PWM maximum possible values are 0 to 255
  analogWrite(enA, 255);
  analogWrite(enB, 255);
  }


  void Car_Backward( void ){
  // Turn on motor A & B 
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
  }
  
 void Car_Forward(void){
  // Now change motor directions
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);
 }

 void Car_Left(void){
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
 }

  void Car_Right(void){
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);
  }

  void Car_Stop(void){
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);
  }
