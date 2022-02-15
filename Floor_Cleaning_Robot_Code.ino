#include <NewPing.h>    //import libraries
#include <LiquidCrystal.h>

const int echo_L = 2;   //initialize pin numbers                              
const int trig_L = 3;
const int echo_M = 4;
const int trig_M = 5;
const int echo_R = 7;
const int trig_R = 8;
const int L1 = 6;
const int L2 = 9;
const int R1 = 10;
const int R2 = 11;
const int button = 12;
const int pump = 13;
int motor_speed = 255;    //speed of the motor can be set between 125 (minimum) and 255 (maximum)
int max_distance = 200;   //max distance of ultrasonic sensors is set to 200cm
int distance_L = 0;
int distance_M = 0;
int distance_R = 0;
char incomingByte;

NewPing sonar_L(trig_L, echo_L, max_distance);    //initialize all the 3 sensors
NewPing sonar_M(trig_M, echo_M, max_distance);
NewPing sonar_R(trig_R, echo_R, max_distance);
LiquidCrystal lcd(A0, A1, A2, A3, A4, A5);    //initialize LCD

void setup()
{  
  pinMode(L1, OUTPUT);    //intitialize pins as output or input
  pinMode(L2, OUTPUT);
  pinMode(R1, OUTPUT);
  pinMode(R2, OUTPUT);
  pinMode(button, INPUT);
  pinMode(pump, OUTPUT);
  digitalWrite(L1, LOW); 
  digitalWrite(L2, LOW);
  digitalWrite(R1, LOW);
  digitalWrite(R2, LOW);
  digitalWrite(pump, LOW);
  lcd.begin(16, 2);
  lcd.print("Welcome!");
  Serial.begin(9600);   //begin serial communication via bluetooth at 9600 baud rate
  delay(2000);
}


void loop()
{
  if(digitalRead(button) == LOW)    //if button is not pressed
  {
    lcd.clear();                    //manual mode
    lcd.print("Manual Mode");
    while(true)
    {
      manualMode();     
      if(digitalRead(button) == HIGH)
      {
        moveStop();
        break;
      }
    }
    delay(100);
  }
  
  else                              //else automatic mode
  {
    lcd.clear();
    lcd.print("Automatic Mode");
    while(true)
    {
      automaticMode();
      if(digitalRead(button) == LOW)
      {
        moveStop();
        break;
      }
    }
    delay(100);
  }
}

void manualMode()
{
  if (Serial.available() > 0)   //check if any data is available
  {
    incomingByte = Serial.read();   //read incoming data
  }

  switch(incomingByte)    //based on received character execute respective commands
  {
    case 'F':
    moveForward();
    lcd.clear();
    lcd.print("Forward");
    incomingByte='*';
    break;
    
    case 'B':
    moveBackward();
    lcd.clear();
    lcd.print("Backward");
    incomingByte='*';
    break;
    
    case 'L':
    moveLeft();
    lcd.clear();
    lcd.print("Left");
    incomingByte='*';
    break;
    
    case 'R':
    moveRight();
    lcd.clear();
    lcd.print("Right");
    incomingByte='*';
    break;
    
    case 'S':
    moveStop();
    lcd.clear();
    lcd.print("Stop");
    incomingByte='*';
    break;
    
    case 'P':
    digitalWrite(pump, HIGH);
    lcd.clear();
    lcd.print("Pump ON");
    incomingByte='*';
    break;
    
    case 'p':
    digitalWrite(pump, LOW); 
    incomingByte='*';
    break;
    
    case '1':
    motor_speed = 155;
    incomingByte='*';
    break;
    
    case '2':
    motor_speed = 205;
    incomingByte='*';
    break;
    
    case '3':
    motor_speed = 255;
    incomingByte='*';
    break;
    
    delay(5000);
  }
}

void automaticMode()
{
  distance_L = readSensor_L();    //read distance from all the 3 sensors
  distance_M = readSensor_M();
  distance_R = readSensor_R();
  lcd.clear();    //print distance on LCD
  lcd.print("L=");
  lcd.print(distance_L);
  lcd.print("cm ");
  lcd.print("M=");
  lcd.print(distance_M);
  lcd.print("cm");
  lcd.setCursor(0, 1);
  lcd.print("R=");
  lcd.print(distance_R);
  lcd.print("cm");
  
  if(distance_M <= 20)    //if middle sensor distance is less than 20cm
  {
    if(distance_R > distance_L)   //check if there is place at right or left
    {
      if((distance_R <= 20) && (distance_L <= 20))    //if there is no place on both sides
      {
        moveStop();
        delay(200);
        moveBackward();   //move back
        delay(2000);
      }
      else
      {
        moveBackward();  //move back then turn right
        delay(500);
        moveRight();
        delay(2000);
      }
    }
    else 
    if(distance_R < distance_L)
    {
      if((distance_R <= 20) && (distance_L <= 20))
      {
        moveStop();   //move back
        delay(200);
        moveBackward();
        delay(2000);
      }
      else
      {
        moveBackward();   //move back then turn left
        delay(500);
        moveLeft();
        delay(2000);
      }
    }
  }
  
  else 
  if(distance_R <= 15)    //if right sensor distance is less than 20cm
  {
    moveLeft();   //turn left
    delay(500);
  }
  else
  if(distance_L <= 15)    //if left sensor distance is less than 20cm
  {
    moveRight();    //turn right
    delay(500);
  }
  else
  {
    moveForward();    //in all other cases keep on moving forward
  }
}

int readSensor_L()    //read distance in centimeters from left sensor
{ 
  delay(70);
  int cm_L = sonar_L.ping_cm();
  if(cm_L==0)
  {
    cm_L = 250;
  }
  return cm_L;
}

int readSensor_M()    //read distance in centimeters from left sensor
{ 
  delay(70);
  int cm_M = sonar_M.ping_cm();
  if(cm_M==0)
  {
    cm_M = 250;
  }
  return cm_M;
}

int readSensor_R()    //read distance in centimeters from left sensor
{ 
  delay(70);
  int cm_R = sonar_R.ping_cm();
  if(cm_R==0)
  {
    cm_R = 250;
  }
  return cm_R;
}

void moveForward()
{
  digitalWrite(L1, LOW); 
  analogWrite(L2, motor_speed);
  analogWrite(R1, motor_speed);
  digitalWrite(R2, LOW);
}

void moveBackward()
{
  analogWrite(L1, motor_speed); 
  digitalWrite(L2, LOW);
  digitalWrite(R1, LOW);
  analogWrite(R2, motor_speed);
}

void moveLeft()
{
  analogWrite(L1, motor_speed); 
  digitalWrite(L2, LOW);
  analogWrite(R1, motor_speed);
  digitalWrite(R2, LOW);
}

void moveRight()
{
  digitalWrite(L1, LOW); 
  analogWrite(L2, motor_speed);
  digitalWrite(R1, LOW);
  analogWrite(R2, motor_speed);
}

void moveStop()
{
  digitalWrite(L1, LOW); 
  digitalWrite(L2, LOW);
  digitalWrite(R1, LOW);
  digitalWrite(R2, LOW);
}
