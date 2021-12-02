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
  String requestBy = getValue(msg, ' ', 0);
  String commands = getValue(msg, ' ', 1);
  String resi = getValue(msg, ' ', 2);
  
  debugln("word 0 = "+requestBy);
  debugln("word 1 = "+commands);
  debugln("word 2 = "+resi);
  
////////////////////////////////////// Bokuno //////////////////////////////////////////////


  if (requestBy == Herocode || requestBy == "BokunoHero")
  {

    //////////////////// ADD ///////////////////
    
    if (commands == "add"){
      {

      readList();
        resi = resi.substring((resi.length()-5),resi.length())+'.';
//        resi.toUpperCase();      
      addResi(countL,"MQTT", resi);

     readList();
        NoResi = "";
        for (int i = 0; i < countL; i++)
        {
          String Resi = "\n" + String(i + 1) + ". --" + getValue(AllResi, '.', i);
          NoResi += Resi;
        }
     NoResi = NoResi.substring(0, NoResi.length()-1);
    
     snprintf (msgi, MSG_BUFFER_SIZE, "List %s",NoResi.c_str() );
     client.publish(pub_user.c_str(),msgi);
    }
    
    }
    /////////////////// OPEN //////////////////////
    else if (getValue(msg, ' ', 1) == "open"){
    bokuOpen("");
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
      readList(); 
      readAddress(); 
      NoResi = "";
        for (int i = 0; i < countL; i++)
        {
          String Resi = "\n" + String(i + 1) + ". --" + getValue(AllResi, '.', i);
          NoResi += Resi;
        }
     NoResi = NoResi.substring(0, NoResi.length()-1);
     sendStatus(NoResi);
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
    readList();
    if (AllResi.indexOf(commands) >= 0){
    bokuOpen(commands);
    delay(5000);
    bokuClose();
    allkey = "";
    isreceive = true;
    
    }
    else if(commands == "ping"){
     readAddress();
    }
    else if(commands == "done"){
      bokuClose();
    }
    blinking();
    
  }
}

void reconnect() {
  // Loop until we're reconnected
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
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
  Wire.begin();
  keypad.begin();
  pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
 servo.attach(2); //D4
 servo.write(80);

delay(800);

  wifi_setting();
    delay(1000);
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  readList();
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
        bokuOpen("");
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


  if (!client.connected()) {
    reconnect();
  }
  client.loop();  

}
