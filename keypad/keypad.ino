#include "Keypad.h"
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


String allkey="";

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

void setup() {
  Serial.begin(9600);
}

void loop() {
  char key = keypad.getKey();

  if (key){
    allkey += key;
    Serial.println(allkey);
    if (allkey == "1234"){
    Serial.println("Open");
    allkey="";
   }
   else if (key == '*'){
    Serial.println("Reset");
    allkey="";
  }
  }
  
}
