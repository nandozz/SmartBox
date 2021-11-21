#include <Keypad_I2C.h>
#include <Keypad.h>          // GDY120705
#include <Wire.h>

#define I2CADDR 0x20

const byte ROWS = 4; //four rows
const byte COLS = 3; //three columns
char keys[ROWS][COLS] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}
  };
byte rowPins[ROWS] = {2, 7, 6, 4}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {3, 1, 5}; 
String allkey,access="";
Keypad_I2C keypad = Keypad_I2C( makeKeymap(keys), rowPins, colPins, ROWS, COLS, I2CADDR );
