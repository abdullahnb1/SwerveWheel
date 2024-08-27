#include <Encoder.h> 

Encoder myEnc(2, 3);

const float valuePerRotation = 900.0; //900 click per rotation
const int wheelRadius = 3;            //3cm radius

int enA = 6;  //Motor enable pin
int m1 = 8;   //Motor pin 1
int m2 = 9;   //Motor pin 2

long currentTime;  //current time in ms

long rotTimeDif;                  //time difference per loop in ms for rotational speed measurement
bool rotSpeedFlag = false;        //rotational speed measurement flag bool
long rotSpeedStepTime;            //time holder for rot speed measurement in ms
long rotSpeedStepDuration = 10;   //time difference between rot measurement in ms
float rotSpeedStepDurationReal;   //real rime difference of rot speed measurements in s
float rotSpeed = 0;               //rotational speed, rotation per second

long oldStepPosition  = 0; //old position container, encoder ticks
long newPosition;          //new position container, encoder ticks
long positionDif;          //position dif between each loop, encoder ticks

long  avrgSpeedTimeDif;            //time difference per loop in ms for average speed 
bool  avrgSpeedFlag = false;       //average speed measurement flag bool
long  avrgSpeedStepTime;           //time holder for average speed measurement in ms
long  avrgSpeedStepDuration = 200;  //time difference between average speed measurement in ms
float avrgWheelSpeed = 0;               //average speed, cm per second
float oldWheelSpeed;               //old wheel speed, cm per second
float newWheelSpeed;               //new wheel speed, cm per second

void setup()
{
  Serial.begin(230400);
  
  pinMode(enA, OUTPUT);
  pinMode(m1, OUTPUT);
  pinMode(m2, OUTPUT);

  currentTime = millis();
  rotSpeedStepTime = currentTime;
  avrgSpeedStepTime = currentTime;
}

void loop()
{

  rotMeasureLoop();
  averageWheelSetupLoop();

  analogWrite(enA, 255);
  digitalWrite(m1, LOW);
  digitalWrite(m2, HIGH);

  Serial.print("Speed:");
  Serial.println(avrgWheelSpeed);

  currentTime = millis();
}


//Speed of the wheel cm / s
float getWheelSpeed()
{
  return rotSpeed*wheelRadius*3.14159*2;
}

void averageWheelSetupLoop()
{
  avrgSpeedTimeDif = currentTime-avrgSpeedStepTime;
  newWheelSpeed = getWheelSpeed();

  if((avrgSpeedTimeDif) >= avrgSpeedStepDuration)
  {
    avrgSpeedStepTime = currentTime;
    avrgSpeedFlag = true;
  }

  if(avrgSpeedFlag)
  {
    avrgWheelSpeed = (newWheelSpeed+oldWheelSpeed)/2;
    oldWheelSpeed = newWheelSpeed;
    avrgSpeedFlag = false;
  }
}

void rotMeasureLoop()
{
  newPosition = myEnc.read();
  rotTimeDif = currentTime-rotSpeedStepTime;
  positionDif = newPosition-oldStepPosition;

  if((rotTimeDif) >= rotSpeedStepDuration)
  {
    rotSpeedStepDurationReal = rotTimeDif/1000.0;

    rotSpeedStepTime = currentTime;
    rotSpeedFlag = true;
  }

  if(rotSpeedFlag)
  {
    rotSpeed = (positionDif/valuePerRotation)/rotSpeedStepDurationReal;

    oldStepPosition = newPosition;

    rotSpeedFlag = false;
  }
}
