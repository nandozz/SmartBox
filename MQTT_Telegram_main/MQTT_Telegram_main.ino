//12/3/21
#define DEBUG 1
#if DEBUG == 1
#define debug(x) Serial.print(x)
#define debugln(x) Serial.println(x)
#else
#define debug(x)
#define debugln(x)
#endif

#include <Arduino.h>
#include <U8g2lib.h>
#include <SPI.h>

U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2(U8G2_R0);

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

  debugln("word 0 = " + requestBy);
  debugln("word 1 = " + commands);
  debugln("word 2 = " + resi);

  ////////////////////////////////////// Bokuno //////////////////////////////////////////////


  if (requestBy == Herocode || requestBy == "BokunoHero")
  {
    //////////////////// ADD ///////////////////

    if (commands == "add") {
      {

        readList();
        resi = resi.substring((resi.length() - 5), resi.length()) + '.';
        //        resi.toUpperCase();
        addResi(countL, "New List", resi);

        readList();
        NoResi = "";
        for (int i = 0; i < countL; i++)
        {
          String Resi = "\n" + String(i + 1) + ". --" + getValue(AllResi, '.', i);
          NoResi += Resi;
        }
        NoResi = NoResi.substring(0, NoResi.length() - 1);

      }

    }
    /////////////////// OPEN //////////////////////
    else if (commands == "open") {
      bokuOpen("");
    }
    //////////////////// CLOSE ///////////////////
    else if (commands == "close") {
      bokuClose();
    }


    //////////////////// RESTART ///////////////////
    else if (commands == "restart") {
        screen("RESTART");
      client.publish(pub_user.c_str(), "RESTART DONE");
      delay(1000);
      debugln("Boku restart");
      ESP.restart();
    }

    //////////////////// RESET ///////////////////
    else if (commands == "reset") {
      client.publish(pub_user.c_str(), "RESET DONE");
      delay(1000);
      debugln("Boku reset");
      resetAll();
    }

    //////////////////// List ///////////////////
    else if (commands == "list") {
      readList();
      readAddress();
      NoResi = "";
      for (int i = 0; i < countL; i++)
      {
        String Resi = "\n" + String(i + 1) + ". --" + getValue(AllResi, '.', i);
        NoResi += Resi;
      }
      NoResi = NoResi.substring(0, NoResi.length() - 1);
      sendStatus(NoResi);
    }
    else if (commands == "ping") {
      client.publish(pub_user.c_str(), "connected");
    }

    //////////////////// CLEAR ///////////////////
    else if (commands == "clear") {
      clearList();
      debugln("Boku No.Resi Clear");
      readList();
    }
    blinking();


    ////////////////////////////// COURIER ///////////////////////////////////
  } else if (getValue(msg, ' ', 0) == "courier") {
    readList();
    if (AllResi.indexOf(commands) >= 0) {
      bokuOpen(commands);
      delay(5000);
      bokuClose();
      allkey = "";
      isreceive = true;

    }
    else if (commands == "ping") {
      readAddress();
    }
    else if (commands == "done") {
      bokuClose();
    }
    blinking();
    screen("No. Resi");

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
  u8g2.begin();
  u8g2.clearBuffer();          // clear the internal memory
  u8g2.setFont(u8g2_font_crox5h_tr);  // choose a suitable font at https://github.com/olikraus/u8g2/wiki/fntlistall
  u8g2.drawStr(18, 29, "Processing"); // write something to the internal memory
  u8g2.sendBuffer();         // transfer internal memory to the display
  delay(10);

  pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  servo.attach(2); //D4
  servo.write(80);

  delay(800);

  wifi_setting();
  delay(1000);
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  readList();
  u8g2.clearBuffer();          // clear the internal memory
  u8g2.setFont(u8g2_font_logisoso28_tr);  // choose a suitable font at https://github.com/olikraus/u8g2/wiki/fntlistall
  u8g2.drawStr(18, 29, "Ready"); // write something to the internal memory
  u8g2.sendBuffer();         // transfer internal memory to the display
  delay(10);
}

void loop() {
  ArduinoOTA.handle();

  //  readkeypad();

  char key = keypad.getKey();

  if (key) {

    digitalWrite(LED_BUILTIN, LOW);
    delay(100);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(100);
    allkey += key;
    debugln(allkey);
    if (key == '*')
    {
    allkey = allkey.substring(0, allkey.length() - 2);
    if (allkey.length() <= 0) {
      bokuClose();
      blinking();
      allkey = "No. Resi";
      }
    }
    screen(allkey);
  }
  
  else if (allkey.length() == 5)
  {
    debugln("in length:" + allkey);
    if (AllResi.indexOf(allkey) >= 0)
    {
      debugln("Match" + AllResi);
      bokuOpen("");
      blinking();
      myBot.sendMessage(GroupID, "--- Received ---\nID: ---" + allkey);
      //          isreceive = true;

      delay(5000);
      bokuClose();
      blinking();
      allkey = "";
      screen("No. Resi");
    }
    else if (allkey == PIN)
    {
      debugln("Open");
      bokuOpen("");
      blinking();
      allkey = "";
      
    }

  }
  if (allkey == "##" + PIN)
  {
    screen("Clear");
    blinking();
    clearList();
    debugln("Boku No.Resi Clear");
    readList();
    screen("No. Resi");
    
  }

  else if (allkey == "#" + PIN + "#")
  {screen("RESET");
  delay(2000);
    blinking();
    myBot.sendMessage(GroupID, "Device ID : " + DevID + "\nGroup ID : " + GroupID + "\n--- Device Reseting ---");
    screen("DONE");
    resetAll();
  }
  if (allkey.length() > 6)
  {
    allkey = "";
    screen("No. Resi");
  }




  if (!client.connected()) {
    reconnect();
  }
  client.loop();

}
