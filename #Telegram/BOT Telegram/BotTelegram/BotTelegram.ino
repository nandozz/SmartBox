//IP address: 192.168.100.10

#include "wifiConfig.h";


#include <ESP8266WiFi.h>
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
  TBMessage msg;

  //Sensor

  //ORDER
  //Read message from UserID

  if (myBot.getNewMessage(msg))
  {
    String Name = (String)msg.sender.firstName;

    String command = msg.text;

    //Command FROM ADMIN
    if (msg.group.id == AdminID){
      
      
      
    if (command == "/alldevices")
    {
      myBot.sendMessage(AdminID, "Device : " + DevNum + " (" + UserID + ")\nName : " + (((String)msg.group.title == "")?(String)msg.sender.firstName : (String)msg.group.title) + "\nAddress : " + Address + "\n--- ACTIVE ---");
    }
    else if (command == "/" + DevNum + "restart")
    {
      myBot.sendMessage(AdminID, "Device : " + DevNum + " (" + UserID + ")\n--- Restart, Are you Sure ? ---\n/"+DevNum+"YES\t\t/"+DevNum+"NO");
      restarting = true;      
      
    }
    else if (restarting && command == "/" + DevNum + "YES")
    {
      myBot.sendMessage(AdminID, "Device : " + DevNum + " (" + UserID + ")\n--- Restart ---");
      myBot.sendMessage(UserID, "Device : " + DevNum + " (" + UserID + ")\n--- Restart by Admin ---");
      restarting = false;      
      ESP.restart();
    }
    
    else if (command == "/" + DevNum + "on")
    {
      myBot.sendMessage(AdminID, "Device : " + DevNum + " (" + UserID + ")\n--- Relay ON ---");
      myBot.sendMessage(UserID, "Device : " + DevNum + " (" + UserID + ")\n--- Relay ON by Admin ---");
      digitalWrite(Relay, LOW);
      fan = true;
    }
    else if (command == "/" + DevNum + "off")
    {
      myBot.sendMessage(AdminID, "Device : " + DevNum + " (" + UserID + ")\n--- Relay OFF ---");
      myBot.sendMessage(UserID, "Device : " + DevNum + " (" + UserID + ")\n--- Relay OFF by Admin ---");
      digitalWrite(Relay, HIGH);
      fan = false;
    }
    else if (command == "/" + DevNum + "reset")
    {
      myBot.sendMessage(AdminID, "Device : " + DevNum + " (" + UserID + ")\n--- RESET, Are you Sure ? ---\n/"+DevNum+"YES\t\t/"+DevNum+"NO");
      reseting = true;   
    }
    else if (reseting && command == "/" + DevNum + "YES")
    {
      myBot.sendMessage(AdminID, "Device : " + DevNum + " (" + UserID + ")\n--- RESET ---");
      myBot.sendMessage(UserID, "Device : " + DevNum + " (" + UserID + ")\n--- RESET by Admin ---");
      reseting = false;   
      resetAll();
    }
    }

    //MESSAGE FROM reg GROUP TELEGRAM
    else if (msg.group.id == UserID)
    {

      if (command == "/order")
      {
        //toAdminID
        myBot.sendMessage(AdminID, "Device : " + DevNum + " (" + int64ToAscii(msg.group.id) + ")\nName : " + (((String)msg.group.title == "")?(String)msg.sender.firstName : (String)msg.group.title) + "\nAddress : " + Address + "\n--- ORDER ---");
        //toUserID
        myBot.sendMessage(UserID, "Terimakasih, " + (String)msg.sender.firstName + "\nPesanan sudah diteruskan ke Admin");
      }

      else if (command == "/on")
      {
        digitalWrite(Relay, LOW);
        myBot.sendMessage(msg.group.id, "Relay Turn ON");
        fan = true;
      }

      else if (command == "/off")
      {
        digitalWrite(Relay, HIGH);
        myBot.sendMessage(msg.group.id, "Relay Turn OFF");
        fan = false;
      }
      else if (command == "/status")
      {
        myBot.sendMessage(UserID, "Device : " + DevNum + " (" + UserID + 
        ")\nStatus Fan : "+(fan ? "ON" : "OFF") + 
        "\nStatus Gas : "+(gas ? "BAHAYA" : "AMAN"));
      }

      else if (command == "/restart" )
      {
        myBot.sendMessage(UserID, "Device : " + DevNum + " (" + UserID + ")\n--- Restart, Are you Sure ? ---\n/YES\t\t/NO");
         restarting = true;       
      }
      else if (restarting && command == "/YES" )
      {
        myBot.sendMessage(UserID, "Device : " + DevNum + " (" + UserID + ")\n--- Device Restart ---");
        restarting = false;
        ESP.restart();
      }
      
      else if (command == "/reset" )
      {
        myBot.sendMessage(UserID, "Device : " + DevNum + " (" + UserID + ")\n--- Reset, Are you Sure ? ---\n/YES\t\t/NO");
         reseting = true;       
      }

      else if (reseting && command == "/YES")
      {
        myBot.sendMessage(UserID, "Device : " + DevNum +
                                      "\nTelegram ID Group : "+UserID+
                                      "\nLakukan registrasi kembali dengan cara :" +
                                      "\n1. Hubungkan perangkat handphone/komputer ke jaringan WiFi \"" + AP + "\"" +
                                      "\n2. Ketik pada browser alamat IP " + IP+
                                      "\nTekan tombol reset apabila belum menerima pesan \"Device Ready\""
                                      "\n\t---RESET DONE---");
                                      reseting = false;
                                      resetAll();
                                      
                                      
      }
      else {
//        myBot.sendMessage(UserID,"Perintah tidak dikenali");
        restarting = false;
        reseting = false;
        }
    }
  }
//  else if (digitalRead(OrderButt) == LOW)
//  {
//    //toAdminID
//    myBot.sendMessage(AdminID, "Device : " + DevNum + "\nAddress : " + Address + "\n--- ORDER ---");
//    //toUserID
//    myBot.sendMessage(UserID, "Pesanan melalui , Device : " + DevNum +"\nSudah diteruskan ke Admin");
//  }
}
