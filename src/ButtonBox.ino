#include <Keypad.h>
#include <Joystick.h>

#define ENABLE_PULLUPS
#define NUMROTARIES 3
#define NUMBUTTONS 18
#define NUMHATS 1
#define NUMROWS 4
#define NUMCOLS 3

const int eBrakeButtonMap = A3;
const int shifterUpButtonMap = 2;
const int shifterDownButtonMap = 3;

byte buttons[NUMROWS][NUMCOLS] = {{0,1,2},{3,4,5},{6,7,8},{9,10,11}};
byte rowPins[NUMROWS] = {A2,A1,A0,15}; 
byte colPins[NUMCOLS] = {14,16,10}; 
Keypad keypad = Keypad(makeKeymap(buttons), rowPins, colPins, NUMROWS, NUMCOLS); 

Joystick_ Joystick(
  JOYSTICK_DEFAULT_REPORT_ID,
  JOYSTICK_TYPE_GAMEPAD,
  NUMBUTTONS, NUMHATS,    // Button Count, Hat Switch Count
  false, false, false,    // X, Y, Z Axis
  false, false, false,    // Rx, Ry, Rz
  false, false,           // rudder or throttle
  false, true, false);    // accelerator, brake, or steering

void setup() {
  pinMode(shifterUpButtonMap, INPUT_PULLUP);
  pinMode(shifterDownButtonMap, INPUT_PULLUP);
  
  Joystick.begin();  
}

void loop() {
  ReadEBrake();  
  ReadShifter();
  ReadButtons();
}

void ReadEBrake() {
  int pot = analogRead(eBrakeButtonMap);
  int mapped = map(pot,0,1023,0,255);
  Joystick.setBrake(mapped);
}

void ReadShifter() {   
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

void ReadButtons() {
  if (keypad.getKeys()){
    for (int i=0; i<LIST_MAX; i++) {
      if (keypad.key[i].stateChanged) {
        switch (keypad.key[i].kstate) {  
          case PRESSED:
          case HOLD:
            Joystick.setButton(keypad.key[i].kchar, 1);
            break;
          case RELEASED:
          case IDLE:
            Joystick.setButton(keypad.key[i].kchar, 0);
            break;
        }
      }   
    }
  }
}

