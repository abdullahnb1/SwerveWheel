unsigned long currentTime;
unsigned long stepTime;
unsigned long stepDuration = 150;

void setup() {
  Serial.begin(9600);
  currentTime = millis();
  stepTime = currentTime;

}
void loop() 
{
  if((currentTime-stepTime) >= stepDuration)
  {
    stepTime = currentTime;
  }


  currentTime = millis();
}
