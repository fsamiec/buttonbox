#include <Joystick.h>

Joystick_ Joystick(
  JOYSTICK_DEFAULT_REPORT_ID,
  JOYSTICK_TYPE_JOYSTICK,
  18, 1,                  // Button Count, Hat Switch Count
  false, false, false,    // X, Y, Z Axis
  false, false, false,    // Rx, Ry, Rz
  false, false,           // rudder or throttle
  false, true, false);    // accelerator, brake, or steering

const int eBrakeButtonMap = A3;

const int shifterUpButtonMap = 2;
const int shifterDownButtonMap = 3;
int shifterLastState[2] = {0,0};

void setup() {
  pinMode(shifterUpButtonMap, INPUT_PULLUP);
  pinMode(shifterDownButtonMap, INPUT_PULLUP);
  
  Joystick.begin();
}

void loop() {
 
  readEBrake();  
  readShifter();

  delay(50);
}

void readEBrake()
{
  int pot = analogRead(eBrakeButtonMap);
  int mapped = map(pot,0,1023,0,255);
  Joystick.setBrake(mapped);
}

void readShifter()
{   
  int upShift = !digitalRead(shifterUpButtonMap);
  int downShift = !digitalRead(shifterDownButtonMap);

  if (upShift) {     
    Joystick.setHatSwitch(0, 180);
  } else if (downShift) {
    Joystick.setHatSwitch(0, 0);
  } else {
    Joystick.setHatSwitch(0, -1);
  }
}