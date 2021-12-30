#include <Servo.h>
Servo servo;
#include <Keypad_I2C.h>
#include <Keypad.h> // GDY120705
#include <Wire.h>


#include <Arduino.h>
#define I2CADDR 0x20
#include <U8g2lib.h>
#include <SPI.h>

U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2(U8G2_R0);

const byte ROWS = 4; //four rows
const byte COLS = 3; //three columns
char keys[ROWS][COLS] = {
    {'1', '2', '3'},//4
    {'4', '5', '6'},//3
    {'7', '8', '9'},//2
    {'*', '0', '#'}};//1
    //7    6    5
byte rowPins[ROWS] = {2, 7, 6, 4}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {3, 1, 5};
//byte rowPins[ROWS] = {4, 3, 2, 1}; //connect to the row pinouts of the keypad
//byte colPins[COLS] = {7, 6, 5};
String allkey = "";
bool checked = false;
#define MSG_BUFFER_SIZE  (150)
char msgi[MSG_BUFFER_SIZE];

void screen(String txt);


void screen(String txt) {
  snprintf(msgi, MSG_BUFFER_SIZE, txt.c_str());
  Serial.println("show " + txt);
  u8g2.clearBuffer();          // clear the internal memory
  u8g2.setFont(u8g2_font_logisoso28_tr);  // choose a suitable font at https://github.com/olikraus/u8g2/wiki/fntlistall
  u8g2.drawStr(3, 29, msgi); // write something to the internal memory
  u8g2.sendBuffer();         // transfer internal memory to the display
  delay(10);
}
Keypad_I2C keypad = Keypad_I2C(makeKeymap(keys), rowPins, colPins, ROWS, COLS, I2CADDR);


int input;
void setup()
{
  pinMode(BUILTIN_LED, OUTPUT); 

  Serial.begin(115200);
  Wire.begin();
  keypad.begin();
  u8g2.begin();
  u8g2.clearBuffer();          // clear the internal memory
  u8g2.setFont(u8g2_font_crox5h_tr);  // choose a suitable font at https://github.com/olikraus/u8g2/wiki/fntlistall
  u8g2.drawStr(3, 25, "Servo"); // write something to the internal memory
  
  u8g2.sendBuffer();         // transfer internal memory to the display
  delay(10);
    servo.attach(2); //D4
  servo.write(120);
  delay(500);

}

void loop() {
  char key = keypad.getKey();
  if (key) {
   
    digitalWrite(LED_BUILTIN, LOW);
    delay(100);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(100);
    allkey += key;
    screen(allkey);

    
  }
  if(key=='#'){
     allkey = allkey.substring(0, allkey.length() - 1);
     screen("deg: "+allkey);
     input = allkey.toInt();

    servo.attach(2); //D4
    delay(5);
    servo.write(input);
    Serial.println(input);
    delay (500);
    servo.detach();
    delay(10);
     digitalWrite(LED_BUILTIN, LOW);
    delay(100);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(100);
     allkey ="";
  }
}
