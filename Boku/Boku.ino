#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "wifiConfig.h";

//String stringOne = "<HTML><HEAD><BODY>";
  

WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE	(50)
char msg[MSG_BUFFER_SIZE];
int value = 0;

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
  Serial.println("word 1 = "+getValue(msg, ' ', 1));
  Serial.println("word 2 = "+getValue(msg, ' ', 2));
  
//Bokuno
  if (getValue(msg, ' ', 0) == Herocode){

    if (getValue(msg, ' ', 1) == "add"){
      
    Serial.println("Save No.Resi "+getValue(msg, ' ', 2));
    }
    else if (getValue(msg, ' ', 1) == "open"){
    Serial.println("Boku Open");
    digitalWrite(LEDindicator, LOW);
//    snprintf (msg, MSG_BUFFER_SIZE, "Boku Open",_);
    Serial.print("Publish message: Boku Open");
    client.publish(publisher.c_str(), "Boku Open");
    }
    else if (getValue(msg, ' ', 1) == "close"){
    Serial.println("Boku Close");
    digitalWrite(LEDindicator, HIGH);
    Serial.print("Publish message: Boku Close");
    client.publish(publisher.c_str(), "Boku Close");
    }
    else if (getValue(msg, ' ', 1) == "restart"){
       client.publish(publisher.c_str(), "RESTART DONE");
    Serial.println("Boku restart");
    ESP.restart();
    }
    else if (getValue(msg, ' ', 1) == "reset"){
       client.publish(publisher.c_str(), "RESET DONE");
    Serial.println("Boku reset");
    resetAll();
    }
  }else if (getValue(msg, ' ', 0) == "courier"){
    if (getValue(msg, ' ', 1) == "AGS"){
      
    Serial.println("No. Resi Benar, Boku Open ");
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
      client.publish(publisher.c_str(), "hello Boku Hero");
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

//  unsigned long now = millis();
//  if (now - lastMsg > 2000) {
//    lastMsg = now;
//    ++value;
//    snprintf (msg, MSG_BUFFER_SIZE, "hello world #%ld", value);
//    Serial.print("Publish message: ");
//    Serial.println(msg);
//    client.publish(publisher.c_str(), msg);
  
}
