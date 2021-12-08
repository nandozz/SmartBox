//BokuBox
#include <CTBot.h>
CTBot myBot;
#include "Utilities.h"
#include "OTA.h";
// #include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <EEPROM.h>
#include <Servo.h>
Servo servo;
WiFiClient espClient;
PubSubClient client(espClient);

unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE  (150)
char msgi[MSG_BUFFER_SIZE];

////////////////////////////////////////////////////
////////////////////////////////////////////////////
String DevID = "A1234";      //Production code
String AP = "Paket Box " + DevID; ////
String APpass = "paketbox";       ////
////////////////////////////////////////////////////
////////////////////////////////////////////////////

//Variables MQTT
bool isopen,isreceive = false;
String Herocode,PIN,NoResi,Address,AllResi = "";
int i = 0;
const char* mqtt_server = "broker.hivemq.com";
String pub_user = "BokuBox/"+Herocode+"/"+DevID;
String pub_courier = "BokuBox/courier/"+DevID;
String subscriber = "BokuBox/+/"+DevID;

String st,content;

//Variables Telegram
String token = "1954149109:AAHtt10ZRb4SwET3RGDFFXg_efCs3PI3d4I"; // REPLACE myToken WITH YOUR TELEGRAM BOT TOKEN
int GroupID,statusCode,countL,lengthmsg;


//ESP1
//int LED_BUILTIN = 0;
//int resetButton = 2;
//ESP8266 12E
//int LED_BUILTIN = 2;
//int resetButton = 4;


//Function Decalration
bool testWifi(void);
void launchWeb(void);
void setupAP(void);
void createWebServer(void);
void resetAll(void);
void blinking(void);
void readAddress(void);
String getValue(void);

//Establishing Local server at port 80 whenever required
ESP8266WebServer server(80);

void wifi_setting()
{
  debugln();
  debugln("Disconnecting previously connected WiFi");
  WiFi.disconnect();
  EEPROM.begin(512); //Initialasing EEPROM
  delay(10);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
//  pinMode(LED_BUILTIN, OUTPUT);
//  digitalWrite(LED_BUILTIN, HIGH);
//  pinMode(resetButton, INPUT);
//  digitalWrite(resetButton, HIGH);
  debugln();
  debugln();
  debugln("Startup");

  //---------------------------------------- Read eeprom for ssid and pass
//  debugln("Reading EEPROM ssid");
  String esid;
  for (int i = 0; i < 32; ++i)
  {
    esid += char(EEPROM.read(i));
  }
  debugln();
  debug("SSID: ");
  debugln(esid);
  
//  debugln("Reading EEPROM pass");
  String epass = "";
  for (int i = 32; i < 96; ++i)
  {
    epass += char(EEPROM.read(i));
  }
  debug("PASS: ");
  debugln(epass);

  //  debugln("Reading EEPROM groupid");
  String egroupid = "";
  for (int i = 96; i < 106; ++i)
  {
    egroupid += char(EEPROM.read(i));
  }
  debug("GroupID: ");
  debugln(egroupid);
  GroupID = egroupid.toInt();


  ////////////////////////////////////////////////////// PIN
  //  debugln("Reading EEPROM PIN");
  String epin = "";
  for (int i = 106; i < 111; ++i)
  {
    epin += char(EEPROM.read(i));
  }
  debug("PIN: ");
  debugln(epin);
  PIN = epin.c_str();

  //  debugln("Reading EEPROM herocode");
  String eherocode = "";
  for (int i = 111; i < 121; ++i)
  {
    eherocode += char(EEPROM.read(i));
  }
  debug("Herocode: ");
  debugln(eherocode);
  Herocode = eherocode.c_str();
  pub_user = "BokuBox/"+Herocode+"/"+DevID;

  /////////////////

  WiFi.begin(esid.c_str(), epass.c_str());
  if (testWifi())
  {
  OTAfunc(esid.c_str(), epass.c_str(),"Boku-esp");


   myBot.wifiConnect(esid.c_str(), epass.c_str());
//set Token
    myBot.setTelegramToken(token);

//Connection wifi check
    if (myBot.testConnection())
    {
      debugln("Success to connect");
      
//      snprintf(msgi, MSG_BUFFER_SIZE, "%s - %s\nBerhasil terdaftar", commands.c_str(), resi.c_str());
//          myBot.sendMessage(GroupID, msgi);
      
      myBot.sendMessage(GroupID, "Device : " + DevID + "\nKey :" + Herocode + "\nGroupID : " + GroupID + "\n--- Device Ready ---");
      
//      myBot.sendMessage(GroupID, "GroupID : " + GroupID + "\nDevice : " + DevID + "\nKey : " + Herocode +  "\n--- Device Ready ---");
      //    digitalWrite(Relay, LOW);
      //buzzer("success");
    }
    
  blinking();
    debugln("Succesfully Connected!!!");
    return;
  }
  else
  {blinking();
  blinking();
    debugln("Turning the HotSpot On");
    launchWeb();
    setupAP();// Setup HotSpot
  }

  debugln();
  debugln("Waiting.");
  int h = 0;
  while ((WiFi.status() != WL_CONNECTED))
  { 
    debug(".");
    delay(100);
    server.handleClient();
    delay(500);
    h++;
    if ( h > 600 ){
      ESP.restart();
    }
    
    
  }

}



//----------------------------------------------- Fuctions used for WiFi credentials saving and connecting to it which you do not need to change 
bool testWifi(void)
{
  int c = 0;
  debugln("Waiting for Wifi to connect");
  while ( c < 40 ) {
    if (WiFi.status() == WL_CONNECTED)
    { 
      return true;
    }
    delay(500);
    debug("*");
    c++;
  }
  debugln("");
  debugln("Connect timed out, opening AP");
  return false;
}

void launchWeb()

{
  debugln("");
  if (WiFi.status() == WL_CONNECTED)
    debugln("WiFi connected");
  debug("Local IP: ");
  debugln(WiFi.localIP());
  debug("SoftAP IP: ");
  debugln(WiFi.softAPIP());
  createWebServer();
  // Start the server
  server.begin();
  debugln("Server started");

}

void setupAP(void)
{
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
  int n = WiFi.scanNetworks();
  debugln("scan done");
  if (n == 0)
    debugln("no networks found");
  else
  {
    debug(n);
    debugln(" networks found");
    for (int i = 0; i < n; ++i)
    {
      // Print SSID and RSSI for each network found
      debug(i + 1);
      debug(": ");
      debug(WiFi.SSID(i));
      debug(" (");
      debug(WiFi.RSSI(i));
      debug(")");
      debugln((WiFi.encryptionType(i) == ENC_TYPE_NONE) ? " " : "*");
      delay(10);
    }
  }
  debugln("");
  st = "<select name='ssid'>";
  for (int i = 0; i < n; ++i)
  {
    // Print SSID and RSSI for each network found
    st += "<option>";
    st += WiFi.SSID(i);
    st += "</option>";
  }
  st += "</select>";
  delay(100);
  
  WiFi.softAP(AP, APpass);  
  debugln("softap");
  launchWeb();
  debugln("over");
//  MDNS.update();
  
}

void resetAll(){
  EEPROM.begin(512);
  // write a 0 to all 512 bytes of the EEPROM
  for (int i = 0; i < 512; i++) {
    EEPROM.write(i, 0);
  }
  EEPROM.end();
  ESP.restart();
}
void clearList(){
  EEPROM.begin(512);
  // write a 0 to all 512 bytes of the EEPROM
  for (int i = 221; i < 512; i++) {
    EEPROM.write(i, 0);
    }
  EEPROM.end();
  myBot.sendMessage(GroupID,"List telah dibersihkan");
  client.publish(pub_user.c_str(), "empty");
}

void readAddress(){
  String eaddress = "";
  for (int i = 121; i < 221; ++i)
  {
    eaddress += char(EEPROM.read(i));
  }
  delay(5);
  debugln();
  debug("Address: ");
  debugln(eaddress);
//  Address = eaddress.c_str();


      snprintf (msgi, MSG_BUFFER_SIZE, "Address : %s",eaddress.c_str() );
   client.publish(pub_user.c_str(),msgi);
   delay(100);
   client.publish(pub_courier.c_str(),msgi);   
//   Address = "";
//   return  Address;
  
}

void readList()
{
  String enoResi = "";
  for (int i = 221; i < 251; ++i)
  {
    enoResi += char(EEPROM.read(i));
  }
  debugln();
  debug("No. Paket : ");
  debug(enoResi + " ");
  
  AllResi = enoResi.c_str();
  delay(100);

  countL = 0;
  int maxIndex = AllResi.length()-1;

  for (int i = 0; i <= maxIndex; i++)
  {
    if (AllResi.charAt(i) == '.' || i == maxIndex)
    {
      countL++;
    }
  }  
}

void addResi(int countL,String commands, String resi){
        if (countL < 5)
        {
          debugln("Save No.Resi : " + resi);
          AllResi += resi;
          
          EEPROM.begin(512);
          for (int i = 0; i < AllResi.length(); ++i)
          {
            EEPROM.write(221 + i, AllResi[i]);
            debugln("Wrote: " + AllResi[i]);
          }

          EEPROM.commit();
          resi = resi.substring(0, resi.length()-1);

          snprintf(msgi, MSG_BUFFER_SIZE, "%s - %s\nBerhasil terdaftar", commands.c_str(), resi.c_str());
          myBot.sendMessage(GroupID, msgi);
          delay(10);
          client.publish(pub_user.c_str(),"List Add");
        }
        else
        {
           String state = "List sudah penuh \nClick http://0af0-180-244-162-11.ngrok.io/add.html?name="+DevID+"&key="+Herocode+"&resi=Clear";
           
          snprintf(msgi, MSG_BUFFER_SIZE,state.c_str());
          myBot.sendMessage(GroupID,msgi);
          delay(10);
          client.publish(pub_user.c_str(),"List Full");
        } 
}

void sendStatus(String NoResi){
  
        String state = "Device ID : " + DevID + "/" + GroupID +
                       "\nStatus BOX : " + (isopen ? "Open" : "Close") +
                       "\nStatus Receive : " + (isreceive ? "Success" : "Not Yet");

        ////////// Telegram
        snprintf(msgi, MSG_BUFFER_SIZE, "%s\nList:%s", state.c_str(), NoResi.c_str());
        myBot.sendMessage(GroupID, msgi);

        ///////// MQTT
        snprintf (msgi, MSG_BUFFER_SIZE, "List.%s",AllResi.c_str() );
        client.publish(pub_user.c_str(),msgi);
      delay(100);
}

void blinking(){
  for(int i = 0;i<2;i++){
  delay(300);
    digitalWrite(LED_BUILTIN, LOW);
     delay(100);
     digitalWrite(LED_BUILTIN, HIGH);
     delay(100);
  }

    debugln("--Blinking LED--");
}

void bokuOpen(String commands){
  isopen = true;
//  unsigned long currentMillis = millis();
//  lastTime = currentMillis; //open begin
  debugln("Box Open");
    delay(10);
    servo.write(0);
    delay(200);

        ////////// Telegram
    if(commands.length()>4)
    {
        snprintf(msgi, MSG_BUFFER_SIZE, "--- New Received ---\nID: ...%s", commands.c_str());
        myBot.sendMessage(GroupID, msgi);
        delay(10);
    }
    client.publish(pub_user.c_str(), "Box Open");
    client.publish(pub_courier.c_str(), "Box Open");
}

void bokuClose(){
  isopen = false;
  debugln("Box Close");
    delay(10);
    servo.write(80);
    delay(200);
    
    client.publish(pub_user.c_str(), "Box Close");
    client.publish(pub_courier.c_str(), "Box Close");
}

void createWebServer()
{
  {
    if (!MDNS.begin("ESP"))
    { //esp.local/
      debugln("MDNS Not responder started");
    }
    server.on("/", []()
              {
                IPAddress ip = WiFi.softAPIP();
                String ipStr = String(ip[0]) + '.' + String(ip[1]) + '.' + String(ip[2]) + '.' + String(ip[3]);
                ////////////////////////////////////////////////
                content = "<!DOCTYPE html>\n";
                content += "<html>\n";
                content += "<head>\n";
                content += "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\n";
                content += "<style>";

                content += "body {flex-direction: column;background-color: #FF4B3A;}";
                content += "h2{width: 284px;height: 125px;left: 45px;top: 77px;font-family: Nunito;font-style: normal;font-weight: 600;font-size: 48px;line-height: 65px;color: rgba(255, 255, 255, 0.6);margin-bottom: 36px;}";
                content += "h1{width: 244px;height: 27px;left: 45px;top: 238px;font-family: Nunito;font-style: normal;font-weight: 800;font-size: 18px;line-height: 25px;color: rgba(255, 255, 255, 0.6);margin-bottom: 36px;}";
                //content +="input,select {font-style: normal;font-weight: 600;font-size: 17px;line-height: 25px;margin-bottom: 20px;border-radius: 10px;height: 35px;padding: 20px;display: block;width: 97%;border: none;background: #FFFFFF;border-radius: 30px;outline: none;}";
                content += "input, select{font-size: 1.2rem;margin-top: 5px;margin-bottom: 20px;border-radius: 10px;height: 35px;padding: 5px;display: block;\n";
                content += "width: 97%;border:none;border-bottom: 1px solid #1890ff;outline: none;}";

                content += "[placeholder]:focus::-webkit-input-placeholder {\n";
                content += "transition: text-indent 0.4s 0.4s ease;text-indent: -100%;opacity: 1;}\n";
                content += "button {font-style: normal;font-weight: 600;font-size: 17px;line-height: 25px;text-align: center;width: 124px;height: 58px;left: 236px;top: 749px;background: #000000;border-radius: 30px;border: none;float: right;color: white;}";

                content += "</style>\n";
                content += "</head>\n";
                content += "<body>\n";

                content += "  <h2>Welcome to\nPaket Box</h2><br><h1>Add Your Device</h1><br>";
                content += "<div class=\"card\">  \n";
                content += "  <div class=\"container\">\n";
                content += "  \t<form  method='get' action='setting'>\n";

                content += "      <input type=\"text\" name=\"groupid\" placeholder=\" Group ID\" maxlength=\"10\" required>\n";
                content += st;
                content += "      <input type=\"text\" name=\"pass\" placeholder=\" Password WiFi\" required>\n";
                content += "      <input type=\"text\" name=\"address\" placeholder=\" Address\" maxlength=\"100\" required>\n";
                content += "      <input type=\"text\" name=\"pin\" placeholder=\"PIN Device\" pattern=\"[0-9]{5}\"  maxlength=\"5\" required>\n";
                content += "      <input type=\"text\" name=\"password\" placeholder=\"create password App\" maxlength=\"10\" required>\n";
                content += "      <button type=\"submit\" >SAVE</button> \n";
                content += "\t</form>   \n";
                content += "  </div>\n";
                content += "</div>\n";
                content += "</body>\n";
                content += "</html> ";

                server.send(200, "text/html", content);
              });

    server.on("/setting", []()
              {
                
                String qgroupid = server.arg("groupid");
                String qsid = server.arg("ssid");
                String qpass = server.arg("pass");
                String qaddress = server.arg("address");
                String qpassapp = server.arg("password");
                String qpin = server.arg("pin");
                if (qsid.length() > 0 && qpass.length() > 0)
                {
                  debugln("clearing eeprom");
                  for (int i = 0; i < 221; ++i)
                  {
                    EEPROM.write(i, 0);
                  }
                  debugln(qsid);
                  debugln("");
                  debugln(qpass);
                  debugln("");
                  debugln(qgroupid);
                  debugln("");
                  debugln(qpassapp);
                  debugln("");
                  debugln(qpin);
                  debugln("");
                  debugln(qaddress);
                  debugln("");

                  debugln("writing eeprom ssid:"); //32 char
                  for (int i = 0; i < qsid.length(); ++i)
                  {
                    EEPROM.write(i, qsid[i]);
                    debug("Wrote: ");
                    debugln(qsid[i]);
                  }
                  debugln("writing eeprom pass:"); //64 char
                  for (int i = 0; i < qpass.length(); ++i)
                  {
                    EEPROM.write(32 + i, qpass[i]);
                    debug("Wrote: ");
                    debugln(qpass[i]);
                  }
                  debugln("writing eeprom groupid:"); //10 char
                  for (int i = 0; i < qgroupid.length(); ++i)
                  {
                    EEPROM.write(96 + i, qgroupid[i]);
                    debug("Wrote: ");
                    debugln(qgroupid[i]);
                  }
                  debugln("writing eeprom PIN:"); //5 char
                  for (int i = 0; i < qpin.length(); ++i)
                  {
                    EEPROM.write(106 + i, qpin[i]);
                    debug("Wrote: ");
                    debugln(qpin[i]);
                  }
                  debugln("writing eeprom Password App:"); //10 char
                  for (int i = 0; i < qpassapp.length(); ++i)
                  {
                    EEPROM.write(111 + i, qpassapp[i]);
                    debug("Wrote: ");
                    debugln(qpassapp[i]);
                  }
                  debugln("writing eeprom address:"); //100 char
                  for (int i = 0; i < qaddress.length(); ++i)
                  {
                    EEPROM.write(121 + i, qaddress[i]);
                    debug("Wrote: ");
                    debugln(qaddress[i]);
                  }

                  EEPROM.commit();

                  content = "<!DOCTYPE html>\n";
                  content += "<html>\n";
                  content += "<head>\n";
                  content += "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\n";
                  content += "<style>";
                  content += "body {flex-direction: column;background-color: #FF4B3A;}";
                  content += "h2{width: 284px;height: 125px;left: 45px;top: 77px;font-family: Nunito;font-style: normal;font-weight: 600;font-size: 48px;line-height: 65px;color: rgba(255, 255, 255, 0.6);margin-bottom: 36px;}";
                  content += "</style>\n";
                  content += "</head>\n";
                  content += "<body>\n";
                  content += "<h2>Setting Successfully</h2>\n<p> ";
                  content += "</body>\n";
                  content += "</html> ";
                  server.send(200, "text/html", content);
                  delay(300);
                  ESP.reset();
                }
                else
                {
                  content = "{\"Error\":\"404 not found\"}";
                  statusCode = 404;
                  debugln("Sending 404");
                }
                server.sendHeader("Access-Control-Allow-Origin", "*");
                server.send(statusCode, "application/json", content);
              });
  }
}

String getValue(String data, char separator, int index)
{
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length() - 1;

  for (int i = 0; i <= maxIndex && found <= index; i++)
  {
    if (data.charAt(i) == separator || i == maxIndex)
    {
      found++;
      strIndex[0] = strIndex[1] + 1;
      strIndex[1] = (i == maxIndex) ? i + 1 : i;
    }
  }

  return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}
