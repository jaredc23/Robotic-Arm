#include <FlexyStepper.h>;

const long maxX = 4500; //max for motor 1: 24000, M2: 4500

const int MOTOR_STEP_PIN = 6;
const int MOTOR_DIRECTION_PIN = 7;
const int emergencyStop = 2;
const int limitSwitch = 3;
const int enablePin = 8;
const int misc = 9;

long p = 0; //Position
long sp = 5000;//Speed       For M1: 10000      M2:5000
long ac = 500;//Acceleration For M1:1000        M2:500

FlexyStepper stepper;

void serialEvent() { //This function handles when there is messages sent through the serial port.
  String b = Serial.readString();
  if(b.indexOf("ready?") != -1)//This is used to check if the Arduino is responsive, without moving anything
  {
    Serial.println("ready");
  }
  else if(b.indexOf("home") != -1)//Homing sequence
  {
    Serial.println("Homing...");
    homeMotor();
    Serial.println("done");
  }
  else if(b.indexOf("move:") != -1)
  {
    long pos = stringToNum(b);
    Serial.println(String(pos));
    moveToPos(pos);
    Serial.println("done");
  }
  else if(b.indexOf("mr") != -1)
  {
    long pos = stringToNum(b);
    if(b.indexOf("-") != -1)
      pos *= -1;
    moveRelPos(pos);
    Serial.println("done");
  }
  else if(b.indexOf("stop") != -1)
  {
    stepper.setTargetPositionInSteps(stepper.getCurrentPositionInSteps());
    Serial.println("Motor Stopped");
    while(Serial.readString().indexOf("continue") == -1);
    Serial.println("done");
  }
  else if(b.indexOf("accel") != -1)
  {
    long accel = stringToNum(b);
    stepper.setAccelerationInStepsPerSecondPerSecond(accel);
    ac = accel;
    Serial.println("done");
  }
  else if(b.indexOf("speed") != -1)
  {
    long acell = stringToNum(b);
    stepper.setSpeedInStepsPerSecond(acell);
    sp = acell;
    Serial.println("done");
  }
  else if(b.indexOf("disable") != -1)
  {
    digitalWrite(enablePin, HIGH);
    Serial.println("done");
  }
  else if(b.indexOf("enable") != -1)
  {
    digitalWrite(enablePin, LOW);
    Serial.println("done");
  }
  
}

long stringToNum(String b)
{
  bool g = false;
  long pos = 0;
  for(int i = 0; i < b.length(); i++)
     if(isDigit(b.charAt(i)))
     {
       g = true;
       pos *= 10;
       pos += b.substring(i, i+1).toInt();
     }
   if(g)
    return pos;
   else
    return -1;
}

void moveRelPos(long pos)
{
  long cur = stepper.getCurrentPositionInSteps();
  if(cur + pos >= maxX)
  {
    moveToPos(maxX);
  }
  else if(cur + pos <= 0)
  {
    moveToPos(0);
  }
  else
  {
    moveToPos(cur + pos);
  }
}

void moveToPos(long pos)
{
  digitalWrite(enablePin, LOW);
  if(p != stepper.getCurrentPositionInSteps())
    stepper.setCurrentPositionInSteps(p);
  p = pos;
  if(pos >=0 && pos <= maxX)
  {
    stepper.setTargetPositionInSteps(pos);
    while(!stepper.motionComplete())
    {
      stepper.processMovement();
      
      if(Serial.available())
      {
        stepper.setTargetPositionInSteps(stepper.getCurrentPositionInSteps());
        serialEvent();
      }
    }
  }
  else
    Serial.println("ERROR: Position of " + String(pos) + " is out of the range of this motor");
  if(pos != p)
    stepper.setCurrentPositionInSteps(p);
    
    digitalWrite(enablePin, HIGH);
}

void homeMotor()
{
  digitalWrite(enablePin, LOW);
  stepper.setSpeedInStepsPerSecond(150); //M2: 150, M1: 500
  stepper.setTargetPositionInSteps(-1.25 * maxX);
  p = -1;
  while(p != 0)
  {
    stepper.processMovement();
    if(Serial.available())
    {
      serialEvent();
    }
  }
  digitalWrite(enablePin, LOW);
  stepper.setTargetPositionRelativeInSteps(100);
  while(!stepper.motionComplete())
  {
    digitalWrite(enablePin, LOW);
    stepper.processMovement();
    if(Serial.available())
    {
      serialEvent();
    }
  }
  stepper.setCurrentPositionInSteps(p);
  stepper.setSpeedInStepsPerSecond(sp);
  digitalWrite(enablePin, HIGH);
}

void limitHit()
{
  p = 0;
  digitalWrite(enablePin, HIGH);
  Serial.println("LIMIT hit at position: " + String(p));
}

void eStop()
{
  digitalWrite(enablePin, HIGH);
  p = stepper.getCurrentPositionInSteps();
  Serial.println(p);
  Serial.println("Motor Stopped");
}

void setup() {
  
  pinMode(MOTOR_STEP_PIN, OUTPUT);
  pinMode(MOTOR_DIRECTION_PIN, OUTPUT);
  pinMode(emergencyStop, INPUT_PULLUP);
  pinMode(limitSwitch, INPUT_PULLUP);
  pinMode(enablePin, OUTPUT);
  pinMode(misc, OUTPUT);

  Serial.begin(115200); //Starting communication port
  Serial.setTimeout(10); //This lowers the timeout so that Serial.readString() doesn't take as long.

  attachInterrupt(digitalPinToInterrupt(limitSwitch), limitHit, FALLING);
  attachInterrupt(digitalPinToInterrupt(emergencyStop), eStop, FALLING);
  
  stepper.connectToPins(MOTOR_STEP_PIN, MOTOR_DIRECTION_PIN);
  stepper.setSpeedInStepsPerSecond(sp);
  stepper.setAccelerationInStepsPerSecondPerSecond(ac);
  stepper.setCurrentPositionInSteps(0);
  
  digitalWrite(enablePin, HIGH); //disables Motor
 // while(!Serial.available()); //Wait for a serial port to be ready
 
  Serial.println("ready");
  
  //digitalWrite(misc, HIGH); //Can be used for a LED to physically indicate that there was a Serial port found
}

void loop() {
  if(Serial.available())
  {
    serialEvent();
  }
}
