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
bool restarting,reseting = false;

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
     String received = readList();   
    
    if (allkey == PIN || received.indexOf(allkey)>=0){
        debugln("Open");        
        bokuOpen();
        blinking();
        if (allkey == PIN){isreceive = true;}
        allkey="";
        }
   else if (allkey == "#"+PIN+"#"){
    myBot.sendMessage(GroupID, "Device ID: " + DevID + " (" + GroupID + ")\n--- Device Reseting ---");
    resetAll();    
   }
  
   else if (key == '*'){
    debugln("Close");
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

//    //Command FROM ADMIN
//    if (msg.group.id == AdminID){
//      
//    if (command == "/alldevices")
//    {
//      myBot.sendMessage(AdminID, "Device : " + DevID + " (" + GroupID + ")\nName : " + (((String)msg.group.title == "")?(String)msg.sender.firstName : (String)msg.group.title) + "\n--- ACTIVE ---");
//    }
//    else if (command == "/" + DevID + "restart")
//    {
//      myBot.sendMessage(AdminID, "Device : " + DevID + " (" + GroupID + ")\n--- Restart, Are you Sure ? ---\n/"+DevID+"YES\t\t/"+DevID+"NO");
//      restarting = true;      
//      
//    }
//    else if (restarting && command == "/" + DevID + "YES")
//    {
//      myBot.sendMessage(AdminID, "Device : " + DevID + " (" + GroupID + ")\n--- Restart ---");
//      myBot.sendMessage(GroupID, "Device : " + DevID + " (" + GroupID + ")\n--- Restart by Admin ---");
//      restarting = false;      
//      ESP.restart();
//    }
//    
//    else if (command == "/" + DevID + "open")
//    {
//      myBot.sendMessage(AdminID, "Device : " + DevID + " (" + GroupID + ")\n--- BOX Open ---");
//      myBot.sendMessage(GroupID, "Device : " + DevID + " (" + GroupID + ")\n--- BOX Open by Admin ---");
//      bokuOpen();
//    }
//    else if (command == "/" + DevID + "close")
//    {
//      myBot.sendMessage(AdminID, "Device : " + DevID + " (" + GroupID + ")\n--- BOX Close ---");
//      myBot.sendMessage(GroupID, "Device : " + DevID + " (" + GroupID + ")\n--- BOX Close by Admin ---");
//      bokuClose();
//    }
//    else if (command == "/" + DevID + "reset")
//    {
//      myBot.sendMessage(AdminID, "Device : " + DevID + " (" + GroupID + ")\n--- RESET, Are you Sure ? ---\n/"+DevID+"YES\t\t/"+DevID+"NO");
//      reseting = true;   
//    }
//    else if (reseting && command == "/" + DevID + "YES")
//    {
//      myBot.sendMessage(AdminID, "Device : " + DevID + " (" + GroupID + ")\n--- RESET ---");
//      myBot.sendMessage(GroupID, "Device : " + DevID + " (" + GroupID + ")\n--- RESET by Admin ---");
//      reseting = false;   
//      resetAll();
//    }
//    
//    }

    //MESSAGE FROM GROUP TELEGRAM
     if (msg.group.id == GroupID)
    {
       int lengthmsg = getValue(command, ' ', 2).length();
       
      //////////////////////////////////////////////////////////////////
      if (command.indexOf("add")>0 && lengthmsg>0){  
        
        String resi = getValue(command, ' ', 2);    
             
        debugln("Save No.Resi : "+resi);    
        clearList();
        
    
        if(lengthmsg > 5 ){
          resi = resi.substring(0,5)+'.';
        }
        else{
          resi = resi+'.';
        }
        AllResi = readList()+resi;
    EEPROM.begin(512);
    for (int i = 0; i < AllResi.length(); ++i)
        {
          EEPROM.write(111 + i, AllResi[i]);
          debugln("Wrote: "+AllResi[i]);
        }
        

        EEPROM.commit();
//        NoResi = getValue(readList(),'.',0);
        
        
        snprintf (msgi, MSG_BUFFER_SIZE, "%s - %s...\nBerhasil terdaftar",getValue(command, ' ', 1).c_str(),getValue(readList(),'.',0).c_str() );
        myBot.sendMessage(GroupID, msgi);
    
    }

      //////////////////////////////////////////////////////////////////


      else if (command.indexOf("remove")>0)
      {
        clearList();
        myBot.sendMessage(GroupID, "Hallo, " + (String)msg.sender.firstName + "\nKamu sudah menghapus paket");
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
      NoResi="";

         for (int i=0;i<3;i++){
        String Resi = "\n"+String(i+1)+" "+getValue(readList(),'.',i)+"..";
        NoResi+=Resi;
        }
      String state="Device ID : " + DevID + "/" + GroupID + 
        "\nStatus BOX : "+(isopen ? "Open" : "Close")+
        "\nPIN : "+PIN.c_str()+
        "\nStatus Receive : "+(isreceive ? "Success" : "Not Yet");
        snprintf (msgi, MSG_BUFFER_SIZE, "%s\nList:%s",state.c_str(),NoResi.c_str() );
        myBot.sendMessage(GroupID, msgi);
      }

      else if (command == "/restart" )
      {
        myBot.sendMessage(GroupID, "Device : " + DevID + " (" + GroupID + ")\n--- Restart, Are you Sure ? ---\n/YES\t\t/NO");
         restarting = true;       
      }
      else if (restarting && command == "/YES" )
      {
        myBot.sendMessage(GroupID, "Device : " + DevID + " (" + GroupID + ")\n--- Device Restarting ---");
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
                                      "\nID Group : "+GroupID+
                                      "\nLakukan registrasi kembali dengan cara :" +
                                      "\n1. Tekan tombol restrart pada device, tunggu beberapa saat"+
                                      "\n2. Hubungkan perangkat handphone/komputer ke jaringan WiFi \"" + AP + "\"" +
                                      "\n3. Ketik pada browser alamat IP " + IP+
                                      "\n4. Isi form yang muncul pada tampilan browser " +
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




//      else if (addresi == true && command.indexOf("-") > 0)
//      {
//        //toAdminID
////        myBot.sendMessage(AdminID, "Device : " + DevID + " (" + int64ToAscii(msg.group.id) + ")\nName : " + (((String)msg.group.title == "")?(String)msg.sender.firstName : (String)msg.group.title) + "\n--- ADD ---");
//        //toGroupID
//        
//        myBot.sendMessage(GroupID, "Paket berhasil terdaftar");
//        
//      }
