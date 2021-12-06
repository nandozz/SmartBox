#include <Keypad_I2C.h>
#include <Keypad.h> // GDY120705
#include <Wire.h>

#define I2CADDR 0x20

const byte ROWS = 4; //four rows
const byte COLS = 3; //three columns
char keys[ROWS][COLS] = {
    {'1', '2', '3'},//4
    {'4', '5', '6'},//3
    {'7', '8', '9'},//2
    {'*', '0', '#'}};//1
    //7    6    5
//byte rowPins[ROWS] = {2, 7, 6, 4}; //connect to the row pinouts of the keypad
//byte colPins[COLS] = {3, 1, 5};
byte rowPins[ROWS] = {4, 3, 2, 1}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {7, 6, 5};
String allkey = "";
bool checked = false;
Keypad_I2C keypad = Keypad_I2C(makeKeymap(keys), rowPins, colPins, ROWS, COLS, I2CADDR);
