#include <Joystick.h>

Joystick_ Joystick(
  JOYSTICK_DEFAULT_REPORT_ID,
  JOYSTICK_TYPE_JOYSTICK,
  12, 0,                  // Button Count, Hat Switch Count
  false, false, false,    // X, Y, Z Axis
  false, false, false,    // Rx, Ry, Rz
  false, false,           // rudder or throttle
  false, true, false);    // accelerator, brake, or steering

const int eBrakeButtonMap = A3;
const int shifterUpButtonMap = 2;
const int shifterDownButtonMap = 3;

int lastButtonState[4] = {0,0};

void setup() {
  pinMode(shifterUpButtonMap, INPUT_PULLUP);
  pinMode(shifterDownButtonMap, INPUT_PULLUP);
  
  Joystick.begin();
}

void loop() {

  int pot = analogRead(eBrakeButtonMap);
  int mapped = map(pot,0,1023,0,255);
  Joystick.setBrake(mapped);
  
  for (int index = 0; index < 2; index++)
  {
    int currentButtonState = !digitalRead(index + shifterUpButtonMap);
    if (currentButtonState != lastButtonState[index])
    {
      Joystick.setButton(index, currentButtonState);
      lastButtonState[index] = currentButtonState;
    }
  }

  delay(50);
}