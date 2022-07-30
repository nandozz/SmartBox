//Use This version
#define DEBUG 0
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
#include <Wire.h>



U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2(U8G2_R0);

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "wifiConfig.h";



void callback(char* topic, byte* payload, unsigned int length) {
  String code = "";
  String msg = "";
  String Resi = "";
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
        //        resi = resi.substring((resi.length() - 5), resi.length()) + '.';
        resi = resi + '.';
        //        resi.toUpperCase();
        addResi(countL, "New List", resi);

        readList();
        NoResi = "";
        for (int i = 0; i < countL; i++)
        {
          Resi = "\n" + String(i + 1) + ". " + getValue(AllResi, '.', i);
          NoResi += Resi;
        }
        NoResi = NoResi.substring(0, NoResi.length() - 1);
        sendStatus("List", NoResi);
        //dataupdate = true;


      }

    }
    /////////////////// OPEN //////////////////////
    else if (commands == "open") {
      bokuOpen(commands);
      delay(7000);
      bokuClose();
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
      //      readAddress();
      NoResi = "";
      for (int i = 0; i < countL; i++)
      {
        Resi = "\n" + String(i + 1) + ". --" + getValue(AllResi, '.', i);
        NoResi += Resi;
      }
      NoResi = NoResi.substring(0, NoResi.length() - 1);
      sendStatus("List", NoResi);
      //dataupdate = true;
    }
    else if (commands == "history") {
      readHistory();
      NoResi = "";
      for (int i = 0; i < countH; i++)
      {
        Resi = "\n" + String(i + 1) + ". --" + getValue(History, '.', i);
        NoResi += Resi;
      }
      NoResi = NoResi.substring(0, NoResi.length() - 1);
      sendStatus("History", NoResi);
      //dataupdate = true;
    }

    if (commands == "ping" || dataupdate) {
      client.publish(pub_user.c_str(), "connected");
      readList();
      delay(10);
      readHistory();
      delay(10);
      readAddress();
      delay(10);

      NoResi = "";
      for (int i = 0; i < countL; i++)
      {
        Resi = "\n" + String(i + 1) + ". " + getValue(AllResi, '.', i);
        NoResi += Resi;
      }
      NoResi = NoResi.substring(0, NoResi.length() - 1);
      sendStatus( "List", NoResi);

      //    dataupdate = false;


    }

    //////////////////// CLEAR ///////////////////
    else if (commands == "clearlist") {
      clearList();
      client.publish(pub_user.c_str(), "listempty");
      debugln("Boku No.Resi Clear");
      readList();
    }
    else if (commands == "clearhis") {
      clearHistory();
      debugln("Boku History Clear");
      readHistory();
    }
    blinking();
    screen("No. Resi");




    ////////////////////////////// COURIER ///////////////////////////////////
  } else if (requestBy == "courier") {
    readList();
    snprintf(msgi, MSG_BUFFER_SIZE, "countL:%s", String(countL).c_str());
      client.publish(pub_user.c_str(), msgi);
    if (AllResi.indexOf(commands) >= 0) {
      bokuOpen(commands);
      Resi = "";
      String readL;
      AllResi = AllResi.substring(0, AllResi.length() - 1);
debugln("Allresi: "+AllResi);
debugln("countL nih: "+String(countL));
      for (int i = 0; i < countL ; i++)
      {
        readL = getValue(AllResi, '.', i);
       
         if (readL.indexOf(commands) >= 0) {
          debugln("Match : "+readL);
          justR = readL;
          delay(10);
        }
        else {
          Resi += readL + ".";
          debugln(Resi);
          delay(10);
          
        }

      }
      clearList();

      //        resi.toUpperCase();
//      Resi = Resi.substring(0, Resi.length() - 1);
      debugln(Resi);
      

      addResi(countL, "New List", Resi);
    
      delay(10);
      readList();
      delay(10);

      sendStatus("List", Resi);

      snprintf(msgi, MSG_BUFFER_SIZE, "JustReceived:%s", justR.c_str());
      client.publish(pub_user.c_str(), msgi);

      delay(7000);
      bokuClose();

    
      //      commands = commands + '.';
      //      addHis(countH, commands);
      //      readHistory();
      //      NoResi = "";
      //      for (int i = 0; i < countH; i++)
      //      {
      //        Resi = "\n" + String(i + 1) + ". --" + getValue(History, '.', i);
      //        NoResi += Resi;
      //      }
      //      NoResi = NoResi.substring(0, NoResi.length() - 1);
      //            sendStatus("History", NoResi);


      //dataupdate = true;
      isreceive = true;

    }
    else if (commands == "ping") {

      client.publish(pub_courier.c_str(), "connected");
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
  client.setServer(mqtt_server.c_str(), 1883);
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
  if (!LittleFS.begin()) {
    debugln("An Error has occurred while mounting LittleFS");
    return;
  }
  Wire.begin();

  pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
 

  wifi_setting();
  delay(1000);
  //  client.setServer(mqtt_server.c_str(), 1883);
  //  client.setCallback(callback);
  readList();
 
}

void loop() {
  ArduinoOTA.handle();

  if (!client.connected()) {
    reconnect();
  }
  client.loop();

}
