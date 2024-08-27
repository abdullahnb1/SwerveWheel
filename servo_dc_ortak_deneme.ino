#include <Encoder.h> 
#include <Servo.h>

// Change these two numbers to the pins connected to your encoder.
//   Best Performance: both pins have interrupt capability
//   Good Performance: only the first pin has interrupt capability
//   Low Performance:  neither pin has interrupt capability
Encoder myEnc(2, 3);
//   avoid using pins with LEDs attached

Servo myservo;  // create servo object to control a servo
// twelve servo objects can be created on most boards

int pos = 0;    // variable to store the servo position


#define m1 8
#define m2 9
#define enA 6

int V_speed = 0;
 
long oldPosition  = -999;
long newPosition; 
long targetPosition = 900*4;
long ERROR = 0;

const double kP =0.3;
const int maxV = 255;
const int minV = 150;
const int limitV = 0.;

int targetAngle=90;
int currentAngle=targetAngle;
bool servoFlag = false;
int servoStepAmount = 1;

unsigned long currentTime;
unsigned long stepTime;
unsigned long stepDuration = 4;

void setup() {
  // put your setup code here, to run on:
  Serial.begin(230400);

  myservo.attach(10);  // attaches the servo on pin 7 to the servo object
  myservo.write(targetAngle);

  Serial.println("Basic Position P Test:");

  pinMode(enA, OUTPUT);
  pinMode(m1, OUTPUT);
  pinMode(m2, OUTPUT);

  targetAngle = 180;
  delay(1000);

  currentTime = millis();
  stepTime = currentTime;
}


void loop() {
  
  currentAngle = myservo.read();

  if((currentTime-stepTime) >= stepDuration)
  {
    stepTime = currentTime;
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

  if(currentAngle == 0)
  {
    targetAngle = 180;
  }
  else if(currentAngle == 180)
  {
    targetAngle = 0;
  }

  newPosition = myEnc.read();
  if (newPosition != oldPosition) {
    oldPosition = newPosition;
    Serial.print("Position:");
    Serial.print(newPosition);
    Serial.print(",");


    Serial.print("Speed Voltage:");
    Serial.print(V_speed);
    Serial.print(",");
  
    Serial.print("Target:");
    Serial.print(targetPosition);
    Serial.print(",");

    Serial.print("ERROR:");
    Serial.println(ERROR);

  }
  ERROR =  targetPosition - newPosition;

  setSpeedV(position_P(ERROR));
  analogWrite(enA, V_speed);
  digitalWrite(m1, LOW);
  digitalWrite(m2, HIGH);

  if(servoFlag) servoFlag = false;
  currentTime = millis();
}



void setSpeedV(int vs){
  if (vs < limitV){
    vs = 0;
  } 
  else if (vs > maxV){
    vs = maxV;
  }
  else if (vs < minV){
    vs = minV;
  } 

  
  V_speed = vs;
}

int position_P(int error)
{
  int output = int(kP * error);
  return output;
}
