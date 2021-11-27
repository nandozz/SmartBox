//ESP12E
// BOT : courierpaketbot 1954149109:AAHtt10ZRb4SwET3RGDFFXg_efCs3PI3d4I
//MQTT
#define DEBUG 0
#if DEBUG == 1
#define debug(x) Serial.print(x)
#define debugln(x) Serial.println(x)
#else
#define debug(x)
#define debugln(x)
#endif

#include <ESP8266WiFi.h>
#include "wifiConfig.h";
#include "i2ckeypadConfig.h"

String IP = "192.168.4.1";
bool restarting = false;
bool reseting = false;
unsigned long loopCount = 0;
unsigned long timert = 0;


void callback(char *topic, byte *payload, unsigned int length)
{
  String code = "";
  String msg = "";
  debug("\nMessage arrived [");
  debug(topic);
  debug("] ");
  for (int i = 0; i < length; i++)
  {
    debug((char)payload[i]);
    code += (char)payload[i];
  }
  debugln();
  msg = String(code);
// msg = courier add 12345
  String requestBy = getValue(msg, ' ', 0);
  String commands = getValue(msg, ' ', 1);
  String resi = getValue(msg, ' ', 2);

  debugln("word 0 = " + getValue(msg, ' ', 0));
  debugln("word 1 = " + getValue(msg, ' ', 1));
  debugln("word 2 = " + getValue(msg, ' ', 2));

  ////////////////////////////////////// Bokuno //////////////////////////////////////////////
  //if (isopen && (currentMillis - lastTime >= interval)) {
  //     debug(lastTime);
  //  debug("     ");
  //  debugln(currentMillis);
  //    bokuClose();
  //  }

  if (requestBy == passApp || requestBy == "BokunoHero")
  {

    //////////////////// ADD ///////////////////

    if (commands == "add")
    {
      addResi(countL,"MQTT", resi);

      NoResi = "";
        for (int i = 0; i < countL; i++)
        {
          String Resi = "\n" + String(i + 1) + ". --" + getValue(AllResi, '.', i);
          NoResi += Resi;
        }
        NoResi = NoResi.substring(0, NoResi.length() - 1);
      sendStatus(countL, NoResi);
      readList();
    }
    /////////////////// OPEN //////////////////////
    else if (commands == "open")
    {
      bokuOpen();
    }
    //////////////////// CLOSE ///////////////////
    else if (commands == "close")
    {
      bokuClose();
    }

    //////////////////// RESTART ///////////////////
    else if (commands == "restart")
    {
      client.publish(pub_user.c_str(), "RESTART DONE");
      delay(1000);
      debugln("Boku restart");
      ESP.restart();
    }

    //////////////////// RESET ///////////////////
    else if (commands == "reset")
    {
      client.publish(pub_user.c_str(), "RESET DONE");
      delay(1000);
      debugln("Boku reset");
      resetAll();
    }

    //////////////////// List ///////////////////
    else if (commands == "listView" || commands == "ping")
    {
      NoResi = "";
        for (int i = 0; i < countL; i++)
        {
          String Resi = "\n" + String(i + 1) + ". --" + getValue(AllResi, '.', i);
          NoResi += Resi;
        }
      NoResi = NoResi.substring(0, NoResi.length() - 1);
      sendStatus(countL, NoResi);
    }

    //////////////////// CLEAR ///////////////////
    else if (commands == "listClear")
    {
      client.publish(pub_user.c_str(), "List -");
      clearList();
      debugln("Boku No.Resi Clear");
  
    }
  }
  ////////////////////////////// COURIER ///////////////////////////////////
  else if (requestBy == "courier")
  {
//  String requestBy = getValue(msg, ' ', 0);
//  String commands = getValue(msg, ' ', 1);
//  String resi = getValue(msg, ' ', 2);
    if (AllResi.indexOf(commands) > 0)
    {
      bokuOpen();
    }
    else if (commands == "ping")
    {
      readAddress();
    }
    else if (commands == "done")
    {
      bokuClose();
    }
    blinking();
  }
}

void reconnect()
{
  // Loop until we're reconnected
  while (!client.connected())
  {
    debug("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "BokuDevice-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str()))
    {
      debugln("connected");
      // Once connected, publish an announcement...
      client.publish(pub_user.c_str(), "Hello, Boku");
      // ... and resubscribe
      client.subscribe(subscriber.c_str());
    }
    else
    {
      debug("failed, rc=");
      debug(client.state());
      debugln(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
      //add reset func if connection failed
    }
  }
}

void setup()
{
  Serial.begin(115200); //Initialising if(DEBUG)Serial Monitor
  Wire.begin();
  keypad.begin();
  pinMode(BUILTIN_LED, OUTPUT); // Initialize the BUILTIN_LED pin as an output
  servo.attach(2);              //D4
  servo.write(80);

  delay(800);
  wifi_setting();

  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop()
{

  ArduinoOTA.handle();
  TBMessage msg;


  //  if ((millis() - timert) > 1000) {
  //    Serial.print("Your loop code ran ");
  //    Serial.print(loopCount);
  //    Serial.println(" times over the last second");
  //    loopCount = 0;
  //    timert = millis();
  //  }
  //  loopCount++;
  ///////////////////READ KEYPAD//////////////////
  char key = keypad.getKey();

  if (key)
  {
    Serial.println(key);
    digitalWrite(LED_BUILTIN, LOW);
    delay(100);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(100);
    allkey += key;
    checked = false;

    debugln("allkey:" + allkey);
  }
  if (key == '*')
  {
    debugln("Close");
    bokuClose();
    allkey = "";
  }
  else if (allkey.length() == 5 && !checked)
  {
    debugln("in length:" + allkey);
    if (AllResi.indexOf(allkey) >= 0)
    {
      debugln("Match" + AllResi);
      bokuOpen();
      myBot.sendMessage(GroupID, "--- Received ---\nID: ---" + allkey);
      //          isreceive = true;

      delay(5000);
      bokuClose();
      allkey = "";
    }
    else if (allkey == PIN)
    {
      debugln("Open");
      bokuOpen();
      allkey = "";
    }
    else
    {
      checked = true;
    }
  }

  else if (allkey == "#" + PIN + "#")
  {
    myBot.sendMessage(GroupID, "Device ID : " + DevID + "\nGroup ID : " + GroupID + "\n--- Device Reseting ---");
    resetAll();
  }
  else if (allkey.length() > 6)
  {
    allkey = "";
  }

  ///////////////////READ MESAGE from Telegram//////////////////
  if (myBot.getNewMessage(msg))
  {
    String Name = (String)msg.sender.firstName;

    String msgTelegram = msg.text;
    // msgTelegram = /add baju 12345

    

    if (msg.group.id == GroupID)
    {
      String item = getValue(msgTelegram, ' ', 1);
      String resi = getValue(msgTelegram, ' ', 2);

      /////////////////////////////MESSAGE FROM GROUP TELEGRAM///////////////////////////////////////
      if (msgTelegram.indexOf("add") > 0 && resi.length() > 4)
      {
        addResi(countL,item,resi);
        NoResi = "";
        for (int i = 0; i < countL; i++)
        {
          String Resi = "\n" + String(i + 1) + ". --" + getValue(AllResi, '.', i);
          NoResi += Resi;
        }
        NoResi = NoResi.substring(0, NoResi.length() - 1);
      sendStatus(countL, NoResi);
        readList();
      }
      

      //      if (command == "/open")

      if (msgTelegram.indexOf("open") > 0)
      {
        bokuOpen();
        myBot.sendMessage(msg.group.id, "BOX OPEN");
        isopen = true;
      }

      else if (msgTelegram.indexOf("close") > 0)
      {
        bokuClose();
        myBot.sendMessage(msg.group.id, "BOX CLOSE");
        isopen = false;
      }

      else if (msgTelegram.indexOf("myprofile") > 0)
      {
        String state = "Device ID : " + DevID + "\nGroup ID : " + GroupID +
                       "\nPIN : " + PIN.c_str();
        snprintf(msgi, MSG_BUFFER_SIZE, "%s", state.c_str());
        myBot.sendMessage(GroupID, msgi);
      }

      else if (msgTelegram.indexOf("notreceived") > 0)
      {
        isreceive = false;
        String state = "Status Receive : Not Yet";
        snprintf(msgi, MSG_BUFFER_SIZE, "%s", state.c_str());
        myBot.sendMessage(GroupID, msgi);
      }

      else if (msgTelegram.indexOf("status") > 0)
      {
        NoResi = "";
        for (int i = 0; i < countL; i++)
        {
          String Resi = "\n" + String(i + 1) + ". --" + getValue(AllResi, '.', i);
          NoResi += Resi;
        }
        NoResi = NoResi.substring(0, NoResi.length() - 1);
        sendStatus(countL, NoResi);
      
      }
      else if (msgTelegram.indexOf("clearlist") > 0)
      {
        clearList();
        //        myBot.sendMessage(GroupID, "Hi, " + (String)msg.sender.firstName + "\nKamu berhasil membersihkan list resi");
        String state = "Hi, " + (String)msg.sender.firstName + "\nList resi telah dihapus";
        snprintf(msgi, MSG_BUFFER_SIZE, state.c_str());
        myBot.sendMessage(GroupID, msgi);
      }

      else if (msgTelegram.indexOf("restart") > 0)
      {
        myBot.sendMessage(GroupID, "Device : " + DevID + " (" + GroupID + ")\n--- Restart, Are you Sure ? ---\n/YES\t\t/NO");
        restarting = true;
      }
      else if (restarting && msgTelegram.indexOf("YES") > 0)
      {
        myBot.sendMessage(GroupID, "Device : " + DevID + " (" + GroupID + ")\n--- Device Restarting ---");
        restarting = false;
        ESP.restart();
      }

      else if (msgTelegram.indexOf("reset") > 0)
      {
        myBot.sendMessage(GroupID, "Device ID: " + DevID + " (" + GroupID + ")\n--- Reset, Are you Sure ? ---\n/YES\t\t/NO");
        reseting = true;
      }

      else if (reseting && msgTelegram.indexOf("YES") > 0)
      {
        myBot.sendMessage(GroupID, "Device : " + DevID +
                                       "\nID Group : " + GroupID +
                                       "\nLakukan registrasi kembali dengan cara :" +
                                       "\n1. Tekan tombol restrart pada device, tunggu beberapa saat" +
                                       "\n2. Hubungkan perangkat handphone/komputer ke jaringan WiFi \"" + AP + "\"" +
                                       "\n3. Ketik pada browser alamat IP " + IP +
                                       "\n4. Isi form yang muncul pada tampilan browser " +
                                       "\nTekan tombol reset apabila belum menerima pesan \"Device Ready\""
                                       "\n\t---RESET DONE---");
        reseting = false;
        resetAll();
      }
      else
      {
        //        myBot.sendMessage(GroupID,"Perintah tidak dikenali");
        restarting = false;
        reseting = false;
      }
    }
  }
    if (!client.connected())
  {
    reconnect();
  }
  client.loop();
}

//      else if (addresi == true && msgTelegram.indexOf("-") > 0)
//      {
//        //toAdminID
////        myBot.sendMessage(AdminID, "Device : " + DevID + " (" + int64ToAscii(msg.group.id) + ")\nName : " + (((String)msg.group.title == "")?(String)msg.sender.firstName : (String)msg.group.title) + "\n--- ADD ---");
//        //toGroupID
//
//        myBot.sendMessage(GroupID, "Paket berhasil terdaftar");
//
//      }
