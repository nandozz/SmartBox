/*
   EEPROM Read

   Reads the value of each byte of the EEPROM and prints it
   to the computer.
   This example code is in the public domain.
*/

#include <EEPROM.h>

// start reading from the first byte (address 0) of the EEPROM
int address = 0;
byte value;

void setup() {
  Serial.begin(115200);  
  EEPROM.begin(512);
  
  // write a 0 to all 512 bytes of the EEPROM
  for (int i = 0; i < 512; i++) {
    EEPROM.write(i, 0);
  }
//
// Serial.println("EEPROM CLEAN");
 
}
  
void loop() {
  
//  Serial.println("Reading EEPROM ssid");

  String esid;
  for (int i = 0; i < 32; ++i)
  {
    esid += char(EEPROM.read(i));
  }
  Serial.println();
  Serial.println();
  Serial.print("SSID: ");
  Serial.println(esid);
//  Serial.println("Reading EEPROM pass");

  String epass = "";
  for (int i = 32; i < 96; ++i)
  {
    epass += char(EEPROM.read(i));
  }
  Serial.print("PASS: ");
  Serial.println(epass);

  String eherocode = "";
  for (int i = 96; i < 106; ++i)
  {
    eherocode += char(EEPROM.read(i));
  }
  Serial.print("Herocode: ");
  Serial.println(eherocode);
//  Herocode = eherocode.c_str();

String eaddress = "";
  for (int i = 106; i < 206; ++i)
  {
    eaddress += char(EEPROM.read(i));
  }
  Serial.println();
  Serial.print("Address: ");
  Serial.println(eaddress);

  String enoResi = "";
  for (int i = 206; i < 211; ++i)
  {
    enoResi += char(EEPROM.read(i));
  }
  Serial.print("No. Paket Terdaftar: ");
  Serial.println(enoResi);

  delay(500);
}
