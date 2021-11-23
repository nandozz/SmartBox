//ESP12E
// BOT : courierpaketbot 1954149109:AAHtt10ZRb4SwET3RGDFFXg_efCs3PI3d4I 

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
#include "i2ckeypadConfig.h"
String IP = "192.168.4.1";
bool restarting = false;
bool reseting = false;
 unsigned long loopCount = 0;
unsigned long timert = 0;

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

  if ((millis() - timert) > 1000) {
    Serial.print("Your loop code ran ");
    Serial.print(loopCount);
    Serial.println(" times over the last second");
    loopCount = 0;
    timert = millis();
  }
  loopCount++;
  ///////////////////READ KEYPAD//////////////////
  char key = keypad.getKey();

 if (key){
  Serial.println(key);
     digitalWrite(LED_BUILTIN, LOW);
     delay(100);
     digitalWrite(LED_BUILTIN, HIGH);
     delay(100);
     allkey += key;
     checked = false;
    
     debugln("allkey:"+allkey);
 }
    if (key == '*'){
    debugln("Close");
    bokuClose();
    allkey="";
  }   
  else if (allkey.length()==5 && !checked){
      debugln("in length:"+allkey);
      if (AllResi.indexOf(allkey)>=0){
        debugln("Match"+AllResi);
          bokuOpen();
        myBot.sendMessage(GroupID, "--- Received ---\nID: ---" + allkey);
//          isreceive = true;
          
        delay(5000);
        bokuClose();
        allkey="";
        }
     else if (allkey == PIN){
        debugln("Open");        
        bokuOpen();
        allkey="";
        }
     else {checked = true;}
  }
   
    else if (allkey == "#"+PIN+"#"){
    myBot.sendMessage(GroupID, "Device ID : " + DevID + "\nGroup ID : " + GroupID + "\n--- Device Reseting ---");
    resetAll();    
   }
   else if(allkey.length()>6 ){
    allkey="";   
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
      if (command.indexOf("add")>0 && lengthmsg>4){  
        
        String resi = getValue(command, ' ', 2);
        debugln("Save No.Resi : "+resi);         
        resi = resi.substring((lengthmsg-5),lengthmsg)+'.';
    
        AllResi = readList()+ resi;
        
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
    {}


      if (command == "/open")
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

      else if (command == "/notreceived")
      {
        isreceive = false;
        String state="Status Receive : Not Yet";
        snprintf (msgi, MSG_BUFFER_SIZE, "%s",state.c_str());
        myBot.sendMessage(GroupID, msgi);
      }
      
      else if (command == "/status")
      {
        AllResi = readList();
        
      NoResi="";

         for (int i=0;i<countList(AllResi,'.');i++){
        String Resi = "\n"+String(i+1)+". --"+getValue(AllResi,'.',i);
        NoResi+=Resi;
        }
        NoResi = NoResi.substring(0,NoResi.length()-1);
      String state="Device ID : " + DevID + "/" + GroupID + 
        "\nStatus BOX : "+(isopen ? "Open" : "Close")+
        "\nStatus Receive : "+(isreceive ? "Success" : "Not Yet");
        snprintf (msgi, MSG_BUFFER_SIZE, "%s\nList:%s",state.c_str(),NoResi.c_str() );
        myBot.sendMessage(GroupID, msgi);
      }
       else if (command=="/clearlist")
      {
        clearList();
//        myBot.sendMessage(GroupID, "Hi, " + (String)msg.sender.firstName + "\nKamu berhasil membersihkan list resi");
        String state = "Hi, " + (String)msg.sender.firstName + "\nList resi telah dihapus";
        snprintf (msgi, MSG_BUFFER_SIZE,  state.c_str() );
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
