#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "wifiConfig.h";


//String stringOne = "<HTML><HEAD><BODY>";
  
//
//WiFiClient espClient;
//PubSubClient client(espClient);
//unsigned long lastMsg = 0;
//#define MSG_BUFFER_SIZE	(50)
//char msgi[MSG_BUFFER_SIZE];
//int value = 0;


void callback(char* topic, byte* payload, unsigned int length) {
  String code = "";
  String msg = "";
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
    code += (char)payload[i];
  }
  Serial.println();
  msg = String(code);
  
  Serial.println("word 0 = "+getValue(msg, ' ', 0));
  Serial.println("word 1 = "+getValue(msg, ' ', 1));
  Serial.println("word 2 = "+getValue(msg, ' ', 2));
  
////////////////////// Bokuno
  if (getValue(msg, ' ', 0) == Herocode||getValue(msg, ' ', 0) == "BokunoHero"){
    
   
    
    
    //////////////////// ADD ///////////////////
    
    if (getValue(msg, ' ', 1) == "add"){      
    Serial.println("Save No.Resi "+getValue(msg, ' ', 2));
    clearList();
    int lengthmsg = getValue(msg, ' ', 2).length() > 5 ? 5 : getValue(msg, ' ', 2).length();
    EEPROM.begin(512);
    for (int i = 0; i < lengthmsg; ++i)
        {
          EEPROM.write(106 + i, getValue(msg, ' ', 2)[i]);
          Serial.print("Wrote: ");
          Serial.println(getValue(msg, ' ', 2)[i]);
        }

        EEPROM.commit();
        readList();
    
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
       client.publish(publisher.c_str(), "RESTART DONE");
       delay(1000);
    Serial.println("Boku restart");
    ESP.restart();
    }
    
    //////////////////// RESET ///////////////////
    else if (getValue(msg, ' ', 1) == "reset"){
       client.publish(publisher.c_str(), "RESET DONE");
       delay(1000);
    Serial.println("Boku reset");
    resetAll();
    }

    
    //////////////////// List ///////////////////
     else if (getValue(msg, ' ', 1) == "listView"){
      readList();
//       client.publish(publisher.c_str(), NoResi.c_str());
//     delay(100);
//      snprintf (msgi, MSG_BUFFER_SIZE, "List %s",NoResi.c_str() );
//   client.publish(subscriber.c_str(),msgi);

    
    }
    
    //////////////////// CLEAR ///////////////////
     else if (getValue(msg, ' ', 1) == "listClear"){
       client.publish(publisher.c_str(), "List Clear");
       clearList();
      Serial.println("Boku No.Resi Clear");
      readList();
    }

////////////////////////////// COURIER ///////////////////////////////////
  }else if (getValue(msg, ' ', 0) == "courier"){

  String enoResi="";
  for (int i = 106; i < 111; ++i)
  {
    enoResi += char(EEPROM.read(i));
  }
  Serial.println();
  Serial.print("No. Paket Terdaftar: ");
  Serial.println(enoResi);
  
    
    if (getValue(msg, ' ', 1) == enoResi){
    bokuOpen();
    }
    else if(getValue(msg, ' ', 1) == "done"){
      bokuClose();
    }
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
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish(publisher.c_str(), "Hello, Boku");
      // ... and resubscribe
      client.subscribe(subscriber.c_str());
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
 servo.attach(2); //D4
//  LEDindicator = 0;

servo.write(0);

delay(2000);
  wifi_setting();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {
  ArduinoOTA.handle();

  if (!client.connected()) {
    reconnect();
  }
  client.loop();  
//unsigned long now = millis();
//  if (now - lastMsg > 2000) {
//    lastMsg = now;
//    ++value;
//      snprintf (msgi, MSG_BUFFER_SIZE, "List : %s",NoResi.c_str() );
//   client.publish(subscriber.c_str(),msgi);
//  }

}
