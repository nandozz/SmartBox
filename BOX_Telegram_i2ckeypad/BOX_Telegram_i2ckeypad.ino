//ESP12E

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
 
void setup() {
  Serial.begin(115200); //Initialising if(DEBUG)Serial Monitor
  Wire.begin();
  keypad.begin(); 
  pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  servo.attach(2); //D4
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
     debugln("allkey:"+allkey);
 }
    if (key == '*'){
    debugln("Close");
    bokuClose();
    allkey="";
    access="";
  }   
  else if (allkey.length()>4){
      access = allkey;
      debugln("in length:"+access);
      if (AllResi.indexOf(access)>=0){
        debugln("Match"+AllResi);
          bokuOpen();
        myBot.sendMessage(GroupID, "--- Received ---\nID: " + access + "...");
          isreceive = true;
          
        delay(5000);
        bokuClose();
        allkey="";
        access="";
        }
     else if (allkey == PIN){
        debugln("Open");        
        bokuOpen();
        allkey="";
        access="";
        }
    else if (allkey == "#"+PIN+"#"){
    myBot.sendMessage(GroupID, "Device ID : " + DevID + "\nGroup ID : " + GroupID + "\n--- Device Reseting ---");
    resetAll();    
   }      
     }
     
///////////////////READ MESAGE//////////////////
if (myBot.getNewMessage(msg))
  {
    String Name = (String)msg.sender.firstName;

    String command = msg.text;
    
    //MESSAGE FROM GROUP TELEGRAM
     if (msg.group.id == GroupID)
    {
       int lengthmsg = getValue(command, ' ', 2).length();
       
      //////////////////////////////////////////////////////////////////
      if (command.indexOf("add")>0 && lengthmsg>0){  
        
        String resi = getValue(command, ' ', 2);
        debugln("Save No.Resi : "+resi);         
    
        if(lengthmsg > 5 ){
          resi = resi.substring(0,5)+'.';
        }
        else{
          resi = resi+'.';
        }
        AllResi = readList()+= resi;
        
    EEPROM.begin(512);
    for (int i = 0; i < AllResi.length(); ++i)
        {
          EEPROM.write(111 + i, AllResi[i]);
          debugln("Wrote: "+AllResi[i]);
        }
        

        EEPROM.commit();
        
        
        snprintf (msgi, MSG_BUFFER_SIZE, "%s - %s\nBerhasil terdaftar",getValue(command, ' ', 1).c_str(),getValue(command,' ',2).c_str() );
        myBot.sendMessage(GroupID, msgi);
    
    }

      //////////////////////////////////////////////////////////////////


      else if (command=="/clear")
      {
        clearList();
        myBot.sendMessage(GroupID, "Hi, " + (String)msg.sender.firstName + "\nKamu berhasil membersihkan list resi");
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

        else if (command == "/myprofile")
      {
        String state="Device ID : " + DevID + "\nGroup ID : " + GroupID + 
                     "\nPIN : "+PIN.c_str();
        snprintf (msgi, MSG_BUFFER_SIZE, "%s",state.c_str());
        myBot.sendMessage(GroupID, msgi);
      }

      else if (command == "/clearreceived")
      {
        isreceive = false;
        String state="Status Receive : No Receive";
        snprintf (msgi, MSG_BUFFER_SIZE, "%s",state.c_str());
        myBot.sendMessage(GroupID, msgi);
      }
      
      else if (command == "/status")
      {
        AllResi = readList();
        
      NoResi="";

         for (int i=0;i<countList(AllResi,'.');i++){
        String Resi = "\n"+String(i+1)+". "+getValue(AllResi,'.',i)+"..";
        NoResi+=Resi;
        }
      String state="Device ID : " + DevID + "/" + GroupID + 
        "\nStatus BOX : "+(isopen ? "Open" : "Close")+
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
