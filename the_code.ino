#include <Encoder.h> 
#include <Servo.h>

#define m1 9
#define m2 8
#define enA 6
#define encoderA 2
#define encoderB 3
#define servoPin 10

Encoder myEnc(encoderA, encoderB);
Servo myservo;  // create servo object to control a servo


const float valuePerRotation = 900.0; //900 click per rotation
const int wheelRadius = 3;            //3cm radius

const float rotToWheelSpeed = wheelRadius*3.14159; //rot to wheel speed constant

long currentTime;  //current time in ms

long oldPosition  = 0;     //old position container, encoder ticks
long newPosition;          //new position container, encoder ticks
long positionDif;          //position dif between each loop, encoder ticks
float vSpeed = 0;          //value for analog write -> 0-255
float rotationCount;       //Rotation amount of the wheel

long  rotTimeDif;                  //time difference per loop in ms for rotational speed measurement
long  rotSpeedStepTime;            //time holder for rot speed measurement in ms
long  rotSpeedStepDuration = 50;   //time difference between rot measurement in ms
float rotSpeedStepDurationReal;    //real rime difference of rot speed measurements in s
float rotSpeed = 0;                //rotational speed, rotation per second
bool  rotSpeedFlag = false;        //rotational speed measurement flag bool
float targetRotSpeed;              //target rot speed
float rotSpeedError;
const float rotKP = 0.4;
const float rotOutputLimit = 0.2;

long  avrgSpeedTimeDif;             //time difference per loop in ms for average speed 
long  avrgSpeedStepTime;            //time holder for average speed measurement in ms
long  avrgSpeedStepDuration = 200;  //time difference between average speed measurement in ms
float avrgWheelSpeed = 0;           //average speed, cm per second
float oldWheelSpeed;                //old wheel speed, cm per second
float newWheelSpeed;                //new wheel speed, cm per second
bool  avrgSpeedFlag = false;        //average speed measurement flag bool
float targetWheelSpeed;               //target wheel speed

int  targetAngle=90;            //servo target angle in degrees
int  currentAngle=targetAngle;  //servo target angle in degrees
int  servoStepAmount = 5;       //servo angle difference per control interval
long servoStepTime;             //time holder for servo control in ms
long servoStepDuration = 20;    //time difference between servo control in ms
bool servoFlag = false;         //servo angle control flag bool

const float maxV = 255.0;
const float minV = 100.0;
const float limitV = 0.04;
const int speedTresholdV = 100;

void setup()
{
  Serial.begin(230400);
  
  myservo.attach(servoPin);  
  myservo.write(targetAngle);

  pinMode(enA, OUTPUT);
  pinMode(m1, OUTPUT);
  pinMode(m2, OUTPUT);

  Serial.println("Hello World!");
  delay(1000);

  //targetAngle = 180; //for servo test
  currentTime = millis();
  rotSpeedStepTime = currentTime;
  avrgSpeedStepTime = currentTime;
  setVSpeed(speedTresholdV);
  setWheelSpeed(10.0);
}

void loop()
{
  rotMeasureLoop();
  averageWheelSetupLoop();
  servoControlLoop();
  wheelSpeedControl();

  Serial.print("avrgWheelSpeed:");
  Serial.print(avrgWheelSpeed);
  Serial.print(",");

  Serial.print("vSpeed:");
  Serial.println(vSpeed);
  
  currentTime = millis();
}


//Speed of the wheel cm / s
float getWheelSpeed()
{
  return rotSpeed*rotToWheelSpeed;
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
  rotationCount = newPosition / valuePerRotation;
  rotTimeDif = currentTime-rotSpeedStepTime;
  positionDif = newPosition-oldPosition;

  if((rotTimeDif) >= rotSpeedStepDuration)
  {
    rotSpeedStepDurationReal = rotTimeDif/1000.0;

    rotSpeedStepTime = currentTime;
    rotSpeedFlag = true;
  }

  if(rotSpeedFlag)
  {
    rotSpeed = (positionDif/valuePerRotation)/rotSpeedStepDurationReal;

    oldPosition = newPosition;

    rotSpeedFlag = false;
  }
}

void controlDC(int v)
{
  int v_abs = abs(v);

  if(v<0)
  {
    analogWrite(enA, v_abs);
    digitalWrite(m1, LOW);
    digitalWrite(m2, HIGH);
  }
  else
  {
    analogWrite(enA, v_abs);
    digitalWrite(m1, HIGH);
    digitalWrite(m2, LOW);
  }

}

void servoControlLoop()
{
  if((currentTime-servoStepTime) >= servoStepDuration)
  {
    servoStepTime = currentTime;
    servoFlag = true;
  }

  if(servoFlag)
  {
    if(currentAngle > targetAngle)
    {
      currentAngle-=servoStepAmount;
      myservo.write(currentAngle);
    }
    else if(currentAngle < targetAngle)
    {
      currentAngle+=servoStepAmount;
      myservo.write(currentAngle);
    }

    servoFlag = false;
  }
}

void servoTest()
{
  if(currentAngle == 0)
  {
    targetAngle = 180;
  }
  else if(currentAngle == 180)
  {
    targetAngle = 0;
  }
}

void setRotSpeed(float rs)
{
  targetRotSpeed = rs;
}

void setWheelSpeed(float ws)
{
  targetWheelSpeed = ws;
}

void wheelSpeedControl()
{
  setRotSpeed(targetWheelSpeed/rotToWheelSpeed);
  rotSpeedControl();
}

void rotSpeedControl()
{
  rotSpeedError = targetRotSpeed - rotSpeed;
  float output = rotKP*rotSpeedError;
  if(output >= rotOutputLimit) output = rotOutputLimit;
  setVSpeed(vSpeed + output);
  controlDC(int(vSpeed));
}

void setVSpeed(float vs)
{
  float vs_abs = abs(vs);

  if (vs_abs < limitV){
    vs_abs = 0;
    //Serial.println("LIMIT vSpeed");
  } 
  else if (vs_abs > maxV){
    vs_abs = maxV;
  }
  else if (vs_abs < minV){
    vs_abs = minV;
  } 

  vSpeed = vs_abs*(vs/vs_abs);
}

