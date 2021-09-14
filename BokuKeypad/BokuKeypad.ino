#define DEBUG 0
#if DEBUG == 1
#define debug(x) Serial.print(x)
#define debugln(x) Serial.println(x)
#else
#define debug(x)
#define debugln(x)
#endif

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "wifiConfig.h";
#include "keypadConfig.h"
// const int FactoryPin = 0; //D3
// int buttonState = 0; 

void callback(char* topic, byte* payload, unsigned int length) {
  String code = "";
  String msg = "";
  debug("\nMessage arrived [");
  debug(topic);
  debug("] ");
  for (int i = 0; i < length; i++) {
    debug((char)payload[i]);
    code += (char)payload[i];
  }
  debugln();
  msg = String(code);
  
  debugln("word 0 = "+getValue(msg, ' ', 0));
  debugln("word 1 = "+getValue(msg, ' ', 1));
  debugln("word 2 = "+getValue(msg, ' ', 2));
  
////////////////////////////////////// Bokuno //////////////////////////////////////////////
//if (isopen && (currentMillis - lastTime >= interval)) {
//     debug(lastTime);
//  debug("     ");
//  debugln(currentMillis);
//    bokuClose();  
//  }

  if (getValue(msg, ' ', 0) == Herocode||getValue(msg, ' ', 0) == "BokunoHero"){
    
    
    
    //////////////////// ADD ///////////////////
    
    if (getValue(msg, ' ', 1) == "add"){      
    debugln("Save No.Resi "+getValue(msg, ' ', 2));
    clearList();
    int lengthmsg = getValue(msg, ' ', 2).length() > 5 ? 5 : getValue(msg, ' ', 2).length();
    EEPROM.begin(512);
    for (int i = 0; i < lengthmsg; ++i)
        {
          EEPROM.write(206 + i, getValue(msg, ' ', 2)[i]);
          debug("Wrote: ");
          debugln(getValue(msg, ' ', 2)[i]);
        }

        EEPROM.commit();
        readList(1);
    
    }
    /////////////////// OPEN //////////////////////
    else if (getValue(msg, ' ', 1) == "open"){
    bokuOpen();
    }
    //////////////////// CLOSE ///////////////////
    else if (getValue(msg, ' ', 1) == "close"){
    bokuClose();
    }
   
    
    //////////////////// RESTART ///////////////////
    else if (getValue(msg, ' ', 1) == "restart"){
       client.publish(pub_user.c_str(), "RESTART DONE");
       delay(1000);
    debugln("Boku restart");
    ESP.restart();
    }
    
    //////////////////// RESET ///////////////////
    else if (getValue(msg, ' ', 1) == "reset"){
       client.publish(pub_user.c_str(), "RESET DONE");
       delay(1000);
    debugln("Boku reset");
    resetAll();
    }
    
    //////////////////// List ///////////////////
     else if (getValue(msg, ' ', 1) == "listView" || getValue(msg, ' ', 1) == "ping"){
      readList(1); 
      readAddress(); 
    }
    
    //////////////////// CLEAR ///////////////////
     else if (getValue(msg, ' ', 1) == "listClear"){
       client.publish(pub_user.c_str(), "List -");
       clearList();
      debugln("Boku No.Resi Clear");
      readList();
    }
    blinking();
    

////////////////////////////// COURIER ///////////////////////////////////
  }else if (getValue(msg, ' ', 0) == "courier"){
    
      
    if (getValue(msg, ' ', 1) == readList(0)){
    bokuOpen();
    }
    else if(getValue(msg, ' ', 1) == "ping"){
     readAddress();
    }
    else if(getValue(msg, ' ', 1) == "done"){
      bokuClose();
    }
    blinking();
    
  }
}


String getValue(String data, char separator, int index)
{
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length()-1;
 
  for(int i=0; i<=maxIndex && found<=index; i++){
    if(data.charAt(i)==separator || i==maxIndex){
        found++;
        strIndex[0] = strIndex[1]+1;
        strIndex[1] = (i == maxIndex) ? i+1 : i;
    }
  } 
 
  return found>index ? data.substring(strIndex[0], strIndex[1]) : "";
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    debug("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "BokuDevice-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      debugln("connected");
      // Once connected, publish an announcement...
      client.publish(pub_user.c_str(), "Hello, Boku");
      // ... and resubscribe
      client.subscribe(subscriber.c_str());
    } else {
      debug("failed, rc=");
      debug(client.state());
      debugln(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
      //add reset func if connection failed
    }
  }
}
 
void setup() {
  
  Serial.begin(115200); //Initialising if(DEBUG)Serial Monitor
  pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
 servo.attach(2); //D4
  //  pinMode(FactoryPin, INPUT);
//  LEDindicator = 0;

servo.write(0);

delay(800);
  wifi_setting();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {
  ArduinoOTA.handle();

//  readkeypad();

  char key = keypad.getKey();

 if (key){
  digitalWrite(LED_BUILTIN, LOW);
     delay(100);
     digitalWrite(LED_BUILTIN, HIGH);
     delay(100);
    allkey += key;
    debugln(allkey);
    
    if (allkey == PIN || allkey == NoResi){
        debugln("Open");
        
        bokuOpen();
        blinking();
        allkey="";
   }
   else if (key == '*' || key == '#'){
    debugln("Reset");
    bokuClose();
    blinking();
    allkey="";
  }
  }
  
  // buttonState = digitalRead(FactoryPin); //D3
  // if (buttonState == LOW){
  //   debugln("Factory Reset");
  //   resetAll();
  //  } 

  if (!client.connected()) {
    reconnect();
  }
  client.loop();  

}
