#include <Encoder.h> 

// Change these two numbers to the pins connected to your encoder.
//   Best Performance: both pins have interrupt capability
//   Good Performance: only the first pin has interrupt capability
//   Low Performance:  neither pin has interrupt capability
Encoder myEnc(2, 3);
//   avoid using pins with LEDs attached


#define m1 8
#define m2 9
#define enA 6

int V_speed = 0;
 
long oldPosition  = -999;
long newPosition; 
long targetPosition = 900;
long ERROR = 0;


const double kP = 0.45;
const int maxV = 255;
const int minV = 130;
const int limitV = 12;


void setup() {
  // put your setup code here, to run on:
  Serial.begin(9600);

  Serial.println("Basic Position P Test:");

  pinMode(enA, OUTPUT);
  pinMode(m1, OUTPUT);
  pinMode(m2, OUTPUT);

  delay(1000);
}


void loop() {

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

int position_P(double error)
{
  int output = int(kP * error);
  return output;
}
