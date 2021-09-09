//new
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
 unsigned long currentMillis; 
 bool isopen = false;

WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE  (150)
char msgi[MSG_BUFFER_SIZE];

////////////////////////////////////////////////////
////////////////////////////////////////////////////
        String BokuID = "A1234";      //Production code
        String AP = "Boku Box "+BokuID;                ////
        String APpass = "bokuboxA1234";             ////
////////////////////////////////////////////////////
////////////////////////////////////////////////////

//Variables
int i = 0;
int statusCode;
String st;
String content;
const char* mqtt_server = "broker.hivemq.com";
String pub_user = "Boku/user/"+BokuID;
String pub_courier = "Boku/courier/"+BokuID;
String subscriber = "Boku/+/"+BokuID;
String Herocode = "";
String NoResi = "";
String Address = "";


//ESP1
//int LED_BUILTIN = 0;
//int resetButton = 2;
//ESP8266 12E
//int LED_BUILTIN = 2;
//int resetButton = 4;


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

  Serial.begin(115200); //Initialising if(DEBUG)Serial Monitor
  Serial.println();
  Serial.println("Disconnecting previously connected WiFi");
  WiFi.disconnect();
  EEPROM.begin(512); //Initialasing EEPROM
  delay(10);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
//  pinMode(LED_BUILTIN, OUTPUT);
//  digitalWrite(LED_BUILTIN, HIGH);
//  pinMode(resetButton, INPUT);
//  digitalWrite(resetButton, HIGH);
  Serial.println();
  Serial.println();
  Serial.println("Startup");

  //---------------------------------------- Read eeprom for ssid and pass
//  Serial.println("Reading EEPROM ssid");
  String esid;
  for (int i = 0; i < 32; ++i)
  {
    esid += char(EEPROM.read(i));
  }
  Serial.println();
  Serial.print("SSID: ");
  Serial.println(esid);
  
//  Serial.println("Reading EEPROM pass");
  String epass = "";
  for (int i = 32; i < 96; ++i)
  {
    epass += char(EEPROM.read(i));
  }
  Serial.print("PASS: ");
  Serial.println(epass);


//  Serial.println("Reading EEPROM herocode");
  String eherocode = "";
  for (int i = 96; i < 106; ++i)
  {
    eherocode += char(EEPROM.read(i));
  }
  Serial.print("Herocode: ");
  Serial.println(eherocode);
  Herocode = eherocode.c_str();


  WiFi.begin(esid.c_str(), epass.c_str());
  if (testWifi())
  {
  OTAfunc(esid.c_str(), epass.c_str(),"Boku-esp");
  blinking();
    Serial.println("Succesfully Connected!!!");
    return;
  }
  else
  {blinking();
  blinking();
    Serial.println("Turning the HotSpot On");
    launchWeb();
    setupAP();// Setup HotSpot
  }

  Serial.println();
  Serial.println("Waiting.");
  int h = 0;
  while ((WiFi.status() != WL_CONNECTED))
  { 
    Serial.print(".");
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
  Serial.println("Waiting for Wifi to connect");
  while ( c < 40 ) {
    if (WiFi.status() == WL_CONNECTED)
    { 
      return true;
    }
    delay(500);
    Serial.print("*");
    c++;
  }
  Serial.println("");
  Serial.println("Connect timed out, opening AP");
  return false;
}

void launchWeb()

{
  Serial.println("");
  if (WiFi.status() == WL_CONNECTED)
    Serial.println("WiFi connected");
  Serial.print("Local IP: ");
  Serial.println(WiFi.localIP());
  Serial.print("SoftAP IP: ");
  Serial.println(WiFi.softAPIP());
  createWebServer();
  // Start the server
  server.begin();
  Serial.println("Server started");

}

void setupAP(void)
{
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
  int n = WiFi.scanNetworks();
  Serial.println("scan done");
  if (n == 0)
    Serial.println("no networks found");
  else
  {
    Serial.print(n);
    Serial.println(" networks found");
    for (int i = 0; i < n; ++i)
    {
      // Print SSID and RSSI for each network found
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.print(WiFi.SSID(i));
      Serial.print(" (");
      Serial.print(WiFi.RSSI(i));
      Serial.print(")");
      Serial.println((WiFi.encryptionType(i) == ENC_TYPE_NONE) ? " " : "*");
      delay(10);
    }
  }
  Serial.println("");
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
  Serial.println("softap");
  launchWeb();
  Serial.println("over");
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
String readList(int mode = 0){
  String enoResi = "";
  for (int i = 206; i < 211; ++i)
  {
    enoResi += char(EEPROM.read(i));
  }
  Serial.println();
  Serial.print("No. Paket : ");
  Serial.print(enoResi+" ");
  Serial.println(enoResi.length());
  NoResi = enoResi.c_str();
  if (mode == 1){  
    snprintf (msgi, MSG_BUFFER_SIZE, ("List %s",NoResi.c_str() ));
    client.publish(pub_user.c_str(),msgi);
      delay(100);
  }
   return  NoResi;
}

String readAddress(){
  String eaddress = "";
  for (int i = 106; i < 206; ++i)
  {
    eaddress += char(EEPROM.read(i));
  }
  Serial.println();
  Serial.print("Address: ");
  Serial.println(eaddress);
  Address = eaddress.c_str();


      snprintf (msgi, MSG_BUFFER_SIZE, "Address %s",Address.c_str() );
   client.publish(pub_user.c_str(),msgi);
      delay(100);
   client.publish(pub_courier.c_str(),msgi);   
   Address = "";
   return  Address;
  
}

void blinking(){
  for(int i = 0;i<3;i++){

    digitalWrite(LED_BUILTIN, LOW);
     delay(100);
     digitalWrite(LED_BUILTIN, HIGH);
     delay(100);
  }

    Serial.println("--Blinking LED--");
}

void bokuOpen(){
  isopen = true;
//  unsigned long currentMillis = millis();
//  lastTime = currentMillis; //open begin
  Serial.println("Box Open");
    delay(100);
    servo.write(160);
    delay(100);
    client.publish(pub_courier.c_str(), "Box Open");
}

void bokuClose(){
  isopen = false;
  Serial.println("Box Close");
    delay(10);
    servo.write(0);
    delay(100);
    client.publish(pub_courier.c_str(), "Box Close");
}

void createWebServer()
{
 {  if (!MDNS.begin("ESP")) { //esp.local/
    Serial.println("MDNS Not responder started");
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
content += "<h3 style=color:#7E9CC0;>Boku ID : "+BokuID+"</h3>\n";
content += "<p> Please select your Access Point</p>";
content +="<div class=\"card\">  \n";
content +="  <div class=\"container\">\n";
content +="  \t<form  method='get' action='setting'>\n";
content +="      <label>SSID</label><br>\n";
content += st;
content +="      <label>Password</label>\n";
content +="      <input type=\"text\" name=\"pass\" placeholder=\" SSID password\" required>\n";

content +="      <label>Device Address</label>\n";
content +="      <input type=\"text\" name=\"address\" placeholder=\"your address\" maxlength=\"100\" required>\n";


content +="      <label>Device Password (max 10 character)</label>\n";
content +="      <input type=\"text\" name=\"password\" placeholder=\"create new password\" maxlength=\"10\" required>\n";


content +="      <button type=\"submit\" >Connect</button> \n";
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
      String qherocode = server.arg("password");
      String qaddress = server.arg("address");
      if (qsid.length() > 0 && qpass.length() > 0) {
        Serial.println("clearing eeprom");
        for (int i = 0; i < 106; ++i) {
          EEPROM.write(i, 0);
        }
        Serial.println(qsid);
        Serial.println("");
        Serial.println(qpass);
        Serial.println("");
        Serial.println(qherocode);
        Serial.println("");
        Serial.println(qaddress);
        Serial.println("");

        Serial.println("writing eeprom ssid:");
        for (int i = 0; i < qsid.length(); ++i)
        {
          EEPROM.write(i, qsid[i]);
          Serial.print("Wrote: ");
          Serial.println(qsid[i]);
        }
        Serial.println("writing eeprom pass:");
        for (int i = 0; i < qpass.length(); ++i)
        {
          EEPROM.write(32 + i, qpass[i]);
          Serial.print("Wrote: ");
          Serial.println(qpass[i]);
        }
        Serial.println("writing eeprom herocode:");
        for (int i = 0; i < qherocode.length(); ++i)
        {
          EEPROM.write(96 + i, qherocode[i]);
          Serial.print("Wrote: ");
          Serial.println(qherocode[i]);
        }
        Serial.println("writing eeprom address:");
        for (int i = 0; i < qaddress.length(); ++i)
        {
          EEPROM.write(106 + i, qaddress[i]);
          Serial.print("Wrote: ");
          Serial.println(qaddress[i]);
        }

        EEPROM.commit();

        content = "{\"Success\":\"saved to eeprom... reset to boot into new wifi\"}";
        statusCode = 200;
        ESP.reset();
      } else {
        content = "{\"Error\":\"404 not found\"}";
        statusCode = 404;
        Serial.println("Sending 404");
      }
      server.sendHeader("Access-Control-Allow-Origin", "*");
      server.send(statusCode, "application/json", content);

    });
  } 
}
