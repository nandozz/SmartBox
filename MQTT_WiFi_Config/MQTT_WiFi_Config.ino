//IP address: 192.168.100.5

#include "wifiConfig.h";


#include <ESP8266WiFi.h>
#include <PubSubClient.h>

String IP = "192.168.4.1";
bool restarting = false;
bool reseting = false;

void setup()
{
settingUp();

}

void loop()
{
  ArduinoOTA.handle();
   if (!client.connected()) {
    reconnect();
  }
//  client.loop();

  unsigned long now = millis();
  if (now - lastMsg > 2000) {
    lastMsg = now;
    ++value;
    snprintf (msg, MSG_BUFFER_SIZE, "hello world #%ld", value);
    Serial.print("Publish message: ");
    Serial.println(msg);
    client.publish(publisher.c_str(), msg);
  }
  

  }

}
