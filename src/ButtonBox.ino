#include <Keypad.h>
#include <Joystick.h>

#define ENABLE_PULLUPS
#define NUMROTARIES 3
#define NUMBUTTONS 18
#define NUMHATS 1
#define NUMROWS 4
#define NUMCOLS 3

/* EBRAKE DEFINITION */
const int eBrakeButtonMap = A3;

/* SHIFTER DEFINITION */
const int shifterUpButtonMap = 2;
const int shifterDownButtonMap = 3;

/* BUTTONS DEFINITION */
byte buttons[NUMROWS][NUMCOLS] = {{0,1,2}, {3,4,5}, {6,7,8}, {9,10,11}};
byte rowPins[NUMROWS] = {A2,A1,A0,15}; 
byte colPins[NUMCOLS] = {14,16,10}; 
Keypad keypad = Keypad(makeKeymap(buttons), rowPins, colPins, NUMROWS, NUMCOLS); 

/* ROTARIES DEFINITION */
struct Rotary {
  byte pin1;
  byte pin2;
  int ccwchar;
  int cwchar;
  volatile unsigned char state;
};

Rotary rotaries[NUMROTARIES] {
  {4,5,12,13,0},
  {6,7,14,15,0},
  {8,9,16,17,0}
};

#define DIR_CCW 0x10
#define DIR_CW 0x20
#define R_START 0x0

#ifdef HALF_STEP
#define R_CCW_BEGIN 0x1
#define R_CW_BEGIN 0x2
#define R_START_M 0x3
#define R_CW_BEGIN_M 0x4
#define R_CCW_BEGIN_M 0x5
const unsigned char ttable[6][4] = {
  {R_START_M,           R_CW_BEGIN,     R_CCW_BEGIN,  R_START},           // R_START (00)
  {R_START_M | DIR_CCW, R_START,        R_CCW_BEGIN,  R_START},           // R_CCW_BEGIN
  {R_START_M | DIR_CW,  R_CW_BEGIN,     R_START,      R_START},           // R_CW_BEGIN
  {R_START_M,           R_CCW_BEGIN_M,  R_CW_BEGIN_M, R_START},           // R_START_M (11)
  {R_START_M,           R_START_M,      R_CW_BEGIN_M, R_START | DIR_CW},  // R_CW_BEGIN_M
  {R_START_M,           R_CCW_BEGIN_M,  R_START_M,    R_START | DIR_CCW}, // R_CCW_BEGIN_M
};
#else
#define R_CW_FINAL 0x1
#define R_CW_BEGIN 0x2
#define R_CW_NEXT 0x3
#define R_CCW_BEGIN 0x4
#define R_CCW_FINAL 0x5
#define R_CCW_NEXT 0x6

const unsigned char ttable[7][4] = {  
  {R_START,    R_CW_BEGIN,  R_CCW_BEGIN, R_START},           // R_START  
  {R_CW_NEXT,  R_START,     R_CW_FINAL,  R_START | DIR_CW},  // R_CW_FINAL  
  {R_CW_NEXT,  R_CW_BEGIN,  R_START,     R_START},           // R_CW_BEGIN  
  {R_CW_NEXT,  R_CW_BEGIN,  R_CW_FINAL,  R_START},           // R_CW_NEXT  
  {R_CCW_NEXT, R_START,     R_CCW_BEGIN, R_START},           // R_CCW_BEGIN  
  {R_CCW_NEXT, R_CCW_FINAL, R_START,     R_START | DIR_CCW}, // R_CCW_FINAL  
  {R_CCW_NEXT, R_CCW_FINAL, R_CCW_BEGIN, R_START},           // R_CCW_NEXT
};
#endif

Joystick_ Joystick(
  JOYSTICK_DEFAULT_REPORT_ID,
  JOYSTICK_TYPE_JOYSTICK,
  NUMBUTTONS, NUMHATS,    // Button Count, Hat Switch Count
  false, false, false,    // X, Y, Z Axis
  false, false, false,    // Rx, Ry, Rz
  false, false,           // rudder or throttle
  false, true, false);    // accelerator, brake, or steering

void setup() {
  pinMode(shifterUpButtonMap, INPUT_PULLUP);
  pinMode(shifterDownButtonMap, INPUT_PULLUP);
  
  SetupRotaries();
  Joystick.begin();  
}

void loop() {
  ReadEBrake();  
  ReadShifter();
  ReadButtons();
  ReadEncoders();
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

void SetupRotaries() {
  for (int i=0; i<NUMROTARIES; i++) {
    pinMode(rotaries[i].pin1, INPUT);
    pinMode(rotaries[i].pin2, INPUT);
    #ifdef ENABLE_PULLUPS
      digitalWrite(rotaries[i].pin1, HIGH);
      digitalWrite(rotaries[i].pin2, HIGH);
    #endif
  }
}

void ReadEncoders() {
  for (int i=0; i<NUMROTARIES; i++) {
    unsigned char result = ProcessRotary(i);
    if (result == DIR_CCW) {
      Joystick.setButton(rotaries[i].ccwchar, 1);
      delay(50); 
      Joystick.setButton(rotaries[i].ccwchar, 0);
    }
    if (result == DIR_CW) {
      Joystick.setButton(rotaries[i].cwchar, 1); 
      delay(50); 
      Joystick.setButton(rotaries[i].cwchar, 0);
    }
  }
}

unsigned char ProcessRotary(int index) {
  unsigned char pinstate = (digitalRead(rotaries[index].pin2) << 1) | digitalRead(rotaries[index].pin1);
  rotaries[index].state = ttable[rotaries[index].state & 0xf][pinstate];
  return (rotaries[index].state & 0x30);
}

