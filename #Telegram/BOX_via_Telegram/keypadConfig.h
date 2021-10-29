#include "Keypad.h"

//#include "wifiConfig.h";
const byte ROWS = 4; //four rows
const byte COLS = 3; //four columns

char keys[ROWS][COLS] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}
};


// For ESP8266 Microcontroller
//byte rowPins[ROWS] = {D2, D7, D6, D0}; 
//byte colPins[COLS] = {D3, D1, D5}; 
byte rowPins[ROWS] = {4, 13, 12, 16}; 
byte colPins[COLS] = {0, 5, 14}; 
String allkey,access="";
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

//void readkeypad() {
//  char key = keypad.getKey();
//
// if (key){
//    allkey += key;
//    debugln(allkey);
//    if (allkey == "12345"){
//    debugln("Open");
//    bokuOpen();
//    allkey="";
//   }
//   else if (key == '*' || '#'){
//    debugln("Reset");
//    bokuClose();
//    allkey="";
//  }
//  }
//}
