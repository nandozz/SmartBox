//BOX_TELEGRAM
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

unsigned long lastTime = 0;  
const long interval = 7000; 
 bool isopen = false;
 bool isreceive = false; 

WiFiClient espClient;
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE  (150)
char msgi[MSG_BUFFER_SIZE];

////////////////////////////////////////////////////
////////////////////////////////////////////////////
        String DevID = "A1234";      //Production code
        String AP = "Paket Box "+DevID;                ////
        String APpass = "paketbox";             ////
////////////////////////////////////////////////////
////////////////////////////////////////////////////

//Variables
int i = 0;
int statusCode;
String st;
String content;

String token = "1934174372:AAEVXbfdOvipT6FOMj4rqgVo0aU8LvW4Ko0"; // REPLACE myToken WITH YOUR TELEGRAM BOT TOKEN
const int AdminID = -508399154;
int GroupID=0;
String PIN = "";
String NoResi = "";


//ESP1
//int LED_BUILTIN = 0;
//int resetButton = 2;
//ESP8266 12E
//int LED_BUILTIN = 2;
int resetButton = 4;


bool state = false;
//Function Decalration
bool testWifi(void);
void launchWeb(void);
void setupAP(void);
void createWebServer(void);
void resetAll(void);
void blinking(void);

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

  //----- Read eeprom for ssid, pass, group id, PIN
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

//  debugln("Reading EEPROM PIN");
  String epin = "";
  for (int i = 106; i < 111; ++i)
  {
    epin += char(EEPROM.read(i));
  }
  debug("PIN: ");
  debugln(epin);
  PIN = epin.c_str();



  ///////////////////////////////////////////////


  WiFi.begin(esid.c_str(), epass.c_str());
  if (testWifi())
  {
  OTAfunc(esid.c_str(), epass.c_str(),"Box-esp");
  myBot.wifiConnect(esid.c_str(), epass.c_str());
  //set Token
  myBot.setTelegramToken(token);
  
  //Connection wifi check
  if (myBot.testConnection())
  {
    Serial.print("Success to connect");
 myBot.sendMessage(GroupID, "Device : " + DevID + " ("+ WiFi.localIP().toString() +")\nGroupID : " + GroupID + "\n--- Device Ready ---");
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
  for (int i = 206; i < 512; i++) {
    EEPROM.write(i, 0);
  }
  EEPROM.end();
}
String readList(){
  String enoResi = "";
  for (int i = 206; i < 211; ++i)
  {
    enoResi += char(EEPROM.read(i));
  }
  debugln();
  debug("No. Paket : ");
  debug(enoResi+" ");
  debugln(enoResi.length());
  NoResi = enoResi.c_str();
//  if (mode == 1){  
//    snprintf (msgi, MSG_BUFFER_SIZE, "List %s",NoResi.c_str() );
//    client.publish(pub_user.c_str(),msgi);
//      delay(100);
//  }
   return  NoResi;
}

//String readAddress(){
//  String eaddress = "";
//  for (int i = 111; i < 211; ++i)
//  {
//    eaddress += char(EEPROM.read(i));
//  }
//  debugln();
//  debug("Address: ");
//  debugln(eaddress);
//  Address = eaddress.c_str();
//
//
//      snprintf (msgi, MSG_BUFFER_SIZE, "Address %s",Address.c_str() );
//   client.publish(pub_user.c_str(),msgi);
//      delay(100);
//   client.publish(pub_courier.c_str(),msgi);   
//   Address = "";
//   return  Address;
//  
//}

void blinking(){
  for(int i = 0;i<2;i++){

    
  delay(300);
    digitalWrite(LED_BUILTIN, HIGH);
     delay(100);
     digitalWrite(LED_BUILTIN, LOW);
     delay(100);
  }

    debugln("--Blinking LED--");
}

void bokuOpen(){
  isopen = true;
//  unsigned long currentMillis = millis();
//  lastTime = currentMillis; //open begin
  debugln("Box Open");
    delay(100);
    servo.write(0);
    delay(100);
    isopen = true;
    blinking();
    
}

void bokuClose(){
  isopen = false;
  debugln("Box Close");
    delay(10);
    servo.write(80);
    delay(100);
    isopen = false;
    blinking();
    
}

void createWebServer()
{
 {  if (!MDNS.begin("ESP")) { //esp.local/
    debugln("MDNS Not responder started");
  }
    server.on("/", []() {

      IPAddress ip = WiFi.softAPIP();
      String ipStr = String(ip[0]) + '.' + String(ip[1]) + '.' + String(ip[2]) + '.' + String(ip[3]);
      /////////////////////////////////////////////////
content = "<!DOCTYPE html>\n";
content += "<html>\n";
content += "<head>\n";
content +="<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\n";
content +="<style>h2{text-align: center;color:#7E9CC0;border: 5px solid #E5F1FF;border-style: none none solid none;}body{display: flex;flex-direction: column;justify-content: center;}";
content +="input, select{font-size: 1.2rem;margin-top: 5px;margin-bottom: 20px;border-radius: 10px;height: 35px;padding: 5px;display: block;\n";
content +="width: 97%;border:none;border-bottom: 1px solid #1890ff;outline: none;}";
content +="[placeholder]:focus::-webkit-input-placeholder {\n";
content +="transition: text-indent 0.4s 0.4s ease;text-indent: -100%;opacity: 1;}\n";
content +=".card {\n";
content +="position: relative;width: 80%;margin-left: auto;margin-right: auto;padding: 5%;border-radius: 20px;\n";
content +="background: linear-gradient(162.54deg, rgba(160, 198, 245, 1) 6.43%, rgba(160, 198, 245, 0) 95.91%);\n";
content +="border: 1px solid #E5E5E5;}\n";
content +=".card:hover {box-shadow: 0 8px 16px 0 rgba(0,0,0,0.2);}\n";
content +="button {\n";
content +="background: #7E9CC0;\n";
content +="border: none;\n";
content +="box-sizing: border-box;\n";
content +="box-shadow: 0px 4px 8px #A0C6F5;\n";
content +="border-radius: 10px;\n";
content +="height: 56px;\n";
content +="display: block;\n";
content +="width: 100%;\n";
content +="color: white;\n";
content +="} \n";
content +="</style>\n";
content +="</head>\n";
content +="<body>\n";
content +="<h2>\""+AP+"\"</h2>\n<p> ";
content += "<h3 style=color:#7E9CC0;>Device ID : "+DevID+"</h3>\n";
content += "<p> Please select your Access Point</p>";
content +="<div class=\"card\">  \n";
content +="  <div class=\"container\">\n";
content +="  \t<form  method='get' action='setting'>\n";
content +="      <label>SSID</label><br>\n";
content += st;
content +="      <label>Password</label>\n";
content +="      <input type=\"text\" name=\"pass\" placeholder=\" SSID password\" required>\n";

//content +="      <label>Device Address</label>\n";
//content +="      <input type=\"text\" name=\"address\" placeholder=\"your address\" maxlength=\"100\" required>\n";


content +="      <label>Group ID (max 10 character)</label>\n";
content +="      <input type=\"text\" name=\"groupid\" placeholder=\"-123456789\" maxlength=\"10\" required>\n";


content +="      <label>PIN (5 digits)</label>\n";
content +="      <input type=\"text\" name=\"pin\" placeholder=\"create new PIN\" pattern=\"[0-9]{5}\"  maxlength=\"5\" required>\n";


content +="      <button type=\"submit\" >SAVE</button> \n";
content +="\t</form>   \n";
content +="  </div>\n";
content +="</div>\n";
content +="</body>\n";
content +="</html> ";


      
      server.send(200, "text/html", content);
    });


    server.on("/setting", []() {
      String qsid = server.arg("ssid");
      String qpass = server.arg("pass");
      String qgroupid = server.arg("groupid");
      String qpin = server.arg("pin");
//      String qaddress = server.arg("address");
      if (qsid.length() > 0 && qpass.length() > 0) {
        debugln("clearing eeprom");
        for (int i = 0; i < 106; ++i) {
          EEPROM.write(i, 0);
        }
        debugln(qsid);
        debugln("");
        debugln(qpass);
        debugln("");
        debugln(qgroupid);
        debugln("");
        debugln(qpin);
        debugln("");
//        debugln(qaddress);
//        debugln("");

        debugln("writing eeprom ssid:");//32 char
        for (int i = 0; i < qsid.length(); ++i)
        {
          EEPROM.write(i, qsid[i]);
          debug("Wrote: ");
          debugln(qsid[i]);
        }
        debugln("writing eeprom pass:");//64 char
        for (int i = 0; i < qpass.length(); ++i)
        {
          EEPROM.write(32 + i, qpass[i]);
          debug("Wrote: ");
          debugln(qpass[i]);
        }
        debugln("writing eeprom groupid:");//10 char
        for (int i = 0; i < qgroupid.length(); ++i)
        {
          EEPROM.write(96 + i, qgroupid[i]);
          debug("Wrote: ");
          debugln(qgroupid[i]);
        }
         debugln("writing eeprom PIN:");//5 char
        for (int i = 0; i < qpin.length(); ++i)
        {
          EEPROM.write(106 + i, qpin[i]);
          debug("Wrote: ");
          debugln(qpin[i]);
        }
//        debugln("writing eeprom address:");//100 char
//        for (int i = 0; i < qaddress.length(); ++i)
//        {
//          EEPROM.write(111 + i, qaddress[i]);
//          debug("Wrote: ");
//          debugln(qaddress[i]);
//        }

        EEPROM.commit();

        
content = "<!DOCTYPE html>\n";
content += "<html>\n";
content += "<head>\n";
content +="<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\n";
content +="<style>h2{text-align: center;color:#7E9CC0;border: 5px solid #E5F1FF;border-style: none none solid none;}body{display: flex;flex-direction: column;justify-content: center;}";
content +="</style>\n";
content +="</head>\n";
content +="<body>\n";
content +="<h2>Setting Successfully</h2>\n<p> ";
content +="</body>\n";
content +="</html> ";
   server.send(200, "text/html", content);
        delay(300);
        ESP.reset();
      } else {
        content = "{\"Error\":\"404 not found\"}";
        statusCode = 404;
        debugln("Sending 404");
      }
      server.sendHeader("Access-Control-Allow-Origin", "*");
      server.send(statusCode, "application/json", content);

    });
  } 
}
