//ESP12E

#define DEBUG 1
#if DEBUG == 1
#define debug(x) Serial.print(x)
#define debugln(x) Serial.println(x)
#else
#define debug(x)
#define debugln(x)
#endif

#include <ESP8266WiFi.h>
#include "wifiConfig.h";
#include "keypadConfig.h"
// const int FactoryPin = 0; //D3
// int buttonState = 0; 
String IP = "192.168.4.1";
bool restarting = false;
bool reseting = false;



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

 
void setup() {
  
  Serial.begin(115200); //Initialising if(DEBUG)Serial Monitor
  pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
 servo.attach(2); //D4
  //  pinMode(FactoryPin, INPUT);
//  LEDindicator = 0;

servo.write(80);

delay(800);
  wifi_setting();
}

void loop() {
  ArduinoOTA.handle();
  TBMessage msg;


///////////////////READ KEYPAD//////////////////
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
        isreceive = true;
        
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




///////////////////READ MESAGE//////////////////
if (myBot.getNewMessage(msg))
  {
    String Name = (String)msg.sender.firstName;

    String command = msg.text;

    //Command FROM ADMIN
    if (msg.group.id == AdminID){
      
      
      
    if (command == "/alldevices")
    {
      myBot.sendMessage(AdminID, "Device : " + DevID + " (" + GroupID + ")\nName : " + (((String)msg.group.title == "")?(String)msg.sender.firstName : (String)msg.group.title) + "\n--- ACTIVE ---");
    }
    else if (command == "/" + DevID + "restart")
    {
      myBot.sendMessage(AdminID, "Device : " + DevID + " (" + GroupID + ")\n--- Restart, Are you Sure ? ---\n/"+DevID+"YES\t\t/"+DevID+"NO");
      restarting = true;      
      
    }
    else if (restarting && command == "/" + DevID + "YES")
    {
      myBot.sendMessage(AdminID, "Device : " + DevID + " (" + GroupID + ")\n--- Restart ---");
      myBot.sendMessage(GroupID, "Device : " + DevID + " (" + GroupID + ")\n--- Restart by Admin ---");
      restarting = false;      
      ESP.restart();
    }
    
    else if (command == "/" + DevID + "open")
    {
      myBot.sendMessage(AdminID, "Device : " + DevID + " (" + GroupID + ")\n--- BOX Open ---");
      myBot.sendMessage(GroupID, "Device : " + DevID + " (" + GroupID + ")\n--- BOX Open by Admin ---");
      bokuOpen();
    }
    else if (command == "/" + DevID + "close")
    {
      myBot.sendMessage(AdminID, "Device : " + DevID + " (" + GroupID + ")\n--- BOX Close ---");
      myBot.sendMessage(GroupID, "Device : " + DevID + " (" + GroupID + ")\n--- BOX Close by Admin ---");
      bokuClose();
    }
    else if (command == "/" + DevID + "reset")
    {
      myBot.sendMessage(AdminID, "Device : " + DevID + " (" + GroupID + ")\n--- RESET, Are you Sure ? ---\n/"+DevID+"YES\t\t/"+DevID+"NO");
      reseting = true;   
    }
    else if (reseting && command == "/" + DevID + "YES")
    {
      myBot.sendMessage(AdminID, "Device : " + DevID + " (" + GroupID + ")\n--- RESET ---");
      myBot.sendMessage(GroupID, "Device : " + DevID + " (" + GroupID + ")\n--- RESET by Admin ---");
      reseting = false;   
      resetAll();
    }
    }

    //MESSAGE FROM GROUP TELEGRAM
    else if (msg.group.id == GroupID)
    {

      if (command == "/add")
      {
        //toAdminID
        myBot.sendMessage(AdminID, "Device : " + DevID + " (" + int64ToAscii(msg.group.id) + ")\nName : " + (((String)msg.group.title == "")?(String)msg.sender.firstName : (String)msg.group.title) + "\n--- ADD ---");
        //toGroupID
        myBot.sendMessage(GroupID, "Terimakasih, " + (String)msg.sender.firstName + "\nPesanan sudah diteruskan ke Admin");
      }

      else if (command == "/open")
      {
        bokuOpen();
        myBot.sendMessage(msg.group.id, "BOX OPEN");
        isopen = true;
        
      }

      else if (command == "/close")
      {
        bokuClose();
        myBot.sendMessage(msg.group.id, "BOX CLOSE");
        isopen = false;
      }
      else if (command == "/status")
      {
        myBot.sendMessage(GroupID, "Device ID : " + DevID + " (" + GroupID + 
        ")\nStatus Receive : "+(isreceive ? "Success" : "Not Yet") + 
        "\nStatus BOX : "+(isopen ? "Open" : "Close"));
      }

      else if (command == "/restart" )
      {
        myBot.sendMessage(GroupID, "Device : " + DevID + " (" + GroupID + ")\n--- Restart, Are you Sure ? ---\n/YES\t\t/NO");
         restarting = true;       
      }
      else if (restarting && command == "/YES" )
      {
        myBot.sendMessage(GroupID, "Device : " + DevID + " (" + GroupID + ")\n--- Device Restart ---");
        restarting = false;
        ESP.restart();
      }
      
      else if (command == "/reset" )
      {
        myBot.sendMessage(GroupID, "Device ID: " + DevID + " (" + GroupID + ")\n--- Reset, Are you Sure ? ---\n/YES\t\t/NO");
         reseting = true;       
      }

      else if (reseting && command == "/YES")
      {
        myBot.sendMessage(GroupID, "Device : " + DevID +
                                      "\nTelegram ID Group : "+GroupID+
                                      "\nLakukan registrasi kembali dengan cara :" +
                                      "\n1. Hubungkan perangkat handphone/komputer ke jaringan WiFi \"" + AP + "\"" +
                                      "\n2. Ketik pada browser alamat IP " + IP+
                                      "\nTekan tombol reset apabila belum menerima pesan \"Device Ready\""
                                      "\n\t---RESET DONE---");
                                      reseting = false;
                                      resetAll();
                                      
                                      
      }
      else {
//        myBot.sendMessage(GroupID,"Perintah tidak dikenali");
        restarting = false;
        reseting = false;
        }
    }
  }

}
