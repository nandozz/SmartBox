//BokuBox
#include <CTBot.h>
CTBot myBot;
#include "Utilities.h"
#include "OTA.h";
#include "LittleFS.h"
#include <ArduinoJson.h>
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
bool isopen, isreceive,dataupdate = false;
String Herocode, PIN, NoResi, Address, AllResi,User, History = "";
int i = 0;
String mqtt_server = "broker.hivemq.com";
String pub_user = "BokuBox/" + Herocode + "/" + DevID;
String pub_courier = "BokuBox/courier/" + DevID;
String subscriber = "BokuBox/+/" + DevID;

String st, content,SSIDwifi, PassWifi;

//Variables Telegram
String token = "1954149109:AAHtt10ZRb4SwET3RGDFFXg_efCs3PI3d4I"; // REPLACE myToken WITH YOUR TELEGRAM BOT TOKEN
int GroupID, statusCode, countL, countH, lengthmsg;


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
void screen(String txt);

///////////

void clearList(void);
void clearHistory(void);
void readList(void);
void readHistory(void);
//void addResi(int countL, String commands, String resi);
//void sendStatus(String NoResi)
void blinking(void);
//void bokuOpen(String commands)
void bokuClose(void);
void createWebServer(void);
//String getValue(String data, char separator, int index)
//void screen(String txt)



//////////q

//Establishing Local server at port 80 whenever required
ESP8266WebServer server(80);

void wifi_setting()
{
  debugln();
  debugln("Disconnecting previously connected WiFi");
  WiFi.disconnect();

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

  debugln("=============config.json==============");
  // READ Config FIle
  File rconf = LittleFS.open("/config.json", "r");
//      while(rconf.available()){
//      Serial.write(rconf.read());
//    }
  if (!rconf) {
    debugln("Failed to open config for reading");
    return;
  }
  StaticJsonDocument<192> doc;
  DeserializationError error = deserializeJson(doc, rconf);
  // Test if parsing succeeds.
  if (error) {
    debug(F("deserializeJson() failed: "));
    debugln(error.f_str());
    return;
  }
    const char* UID = doc["UID"]; // "-1231231"
    const char* SSIDs = doc["SSID"]; // "WIFI"
    const char* wfpas = doc["wfpas"]; // "qwerqwer"
    const char* key = doc["key"]; // "pas"
    const char* pin = doc["PIN"]; // "12345"
    const char* address = doc["address"]; // "Perumahan...."

//  debugln(SSIDs);
//  debugln(wfpas);
//  debugln(address);

  rconf.close();
  debugln("============Config Close===============");

  debugln("=============product.json==============");
  // READ Product FIle
  File rprod = LittleFS.open("/product.json", "r");
//      while(rprod.available()){
//      Serial.write(rprod.read());
//    }
  if (!rprod) {
    debugln("Failed to open  rprod for reading");
    return;
  }
  //Open the  prod
    StaticJsonDocument<768> pro;
    error = deserializeJson(pro,rprod);
  // Test if parsing succeeds.
  if (error) {
    debug(F("deserializeJson() failed: "));
    debugln(error.f_str());
    return;
  }
    const char* DeviceID = pro["DeviceID"]; // "A1234"
    const char* APoint = pro["APoint"]; // "BokuBox"
    const char* APpas = pro["APpas"]; // "paketbox"
    const char* IP = pro["IP"]; // "192.168.4.1"
    const char* username = pro["username"]; // "NANDO"
    const char* password = pro["password"]; // "Hivemq123"
    const char* broker = pro["broker"]; // "broker.hivemq.com"
    bool SSL = pro["SSL"]; // false
    const char* PORT = pro["PORT"]; // "8884"
    const char* tokenTG = pro["token"]; // "Telegram BOT"
    const char* pubuser = pro["pubuser"]; // "BokuBox/"
    const char* pubcourier = pro["pubcourier"]; // "BokuBox/courier/"
    const char* subs = pro["subscriber"]; // "BokuBox/+/"
    const char* ver = pro["version"]; // "v.1"
    const char* productID = pro["productID"]; // "A1234-121221-1"
    const char* warantyvalid = pro["warantyvalid"]; // "12-3-22"

//  debugln(username);
//  debugln(tokenTG);
//  debugln(warantyvalid);
 
   rprod.close();
  debugln("============Product Close===============");

  ///////////////////////////Setup global variable////////////////////////////////////////
  DevID = DeviceID;      //Production code
  AP = String(APoint) + " " + String(DeviceID);
  APpass = String(APpas);

  GroupID = String(UID).toInt();
  token = tokenTG;
  
  PIN = pin;
  Herocode = key;
  Address = address;
 
  pub_user = pubuser + Herocode + "/" + DevID;
  pub_courier = pubcourier + DevID;
  subscriber = subs + DevID;
  mqtt_server = broker;

  SSIDwifi=SSIDs;
  PassWifi=wfpas;
  
//  debugln(SSIDwifi);
//  debugln(PassWifi);


  WiFi.begin(SSIDwifi.c_str(),PassWifi.c_str());
  debugln("WiFi Begin");
  if (testWifi())
  {
    OTAfunc(SSIDwifi.c_str(), PassWifi.c_str(), "Boku-esp");

    myBot.wifiConnect(SSIDwifi.c_str(), PassWifi.c_str());
    //set Token
    myBot.setTelegramToken(token);

    //Connection wifi check
    if (myBot.testConnection())
    {
      debugln("Success to connect");
      myBot.sendMessage(GroupID, "Device : " + DevID + "\nKey : " + Herocode + "\nGroupID : " + GroupID + "\n--- Device Ready ---");

      //      myBot.sendMessage(GroupID, "GroupID : " + GroupID + "\nDevice : " + DevID + "\nKey : " + Herocode +  "\n--- Device Ready ---");
      //    digitalWrite(Relay, LOW);
      //buzzer("success");
    }

    blinking();
    debugln("Succesfully Connected!!!");
    return;
  }
  else
  {
    u8g2.clearBuffer();          // clear the internal memory
    u8g2.setFont(u8g2_font_crox5h_tr);  // choose a suitable font at https://github.com/olikraus/u8g2/wiki/fntlistall
    u8g2.drawStr(3, 25, "192.168.4.1"); // write something to the internal memory
    u8g2.sendBuffer();         // transfer internal memory to the display
    delay(10);
    blinking();
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
    if ( h > 600 ) {
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

void resetAll() {
  File list = LittleFS.open("/list.txt", "w+");
  list.print("");
  delay(10);
  list.close();
  delay(10);
  File his = LittleFS.open("/history.json", "w+");
  his.print("");
  delay(10);
  his.close();
  delay(10);
  
  StaticJsonDocument<768> doc;
  doc["UID"] = "-465154529";
  doc["SSID"] = "WIFI";
  doc["wfpas"] = "password";
  doc["key"] = "pas";
  doc["PIN"] = "12345";
  doc["address"] = "Blok A1.1";

  // write to file
  File wconf = LittleFS.open("/config.json", "w+");
  serializeJson(doc, wconf);
  serializeJsonPretty(doc, Serial);
  wconf.println();
  wconf.close();
  delay(10);
  ESP.restart();
}


void readAddress() {
  debug("Address: ");
  debugln(Address);

  snprintf (msgi, MSG_BUFFER_SIZE, "Address : %s", Address.c_str() );
  client.publish(pub_user.c_str(), msgi);
  delay(100);
  client.publish(pub_courier.c_str(), msgi);

}

void clearList() {

  debugln("Clear List");
  File clis = LittleFS.open("/list.txt", "w+");

  //Write to the file
  clis.print("");
  delay(1);
  //Close the lis
  clis.close();

  myBot.sendMessage(GroupID, "List telah dibersihkan");
  client.publish(pub_user.c_str(), "listempty");
}

void clearHistory() {

  debugln("Clear History");
  File chis = LittleFS.open("/history.txt", "w+");

  //Write to the file
  chis.print("");
  delay(1);
  //Close the chis
  chis.close();

  myBot.sendMessage(GroupID, "History telah dibersihkan");
  client.publish(pub_user.c_str(), "historyempty");
}

void readList()
{
  File rlis = LittleFS.open("/list.txt", "r");
  if (!rlis) {
    //Read the rlis data and display it on LCD
    debugln("list.txt doesn't exist!");
    return;
  }
  AllResi = rlis.readString();
  debugln(AllResi);
  rlis.close();

  debugln();
  debug("List Content: ");
  debug(AllResi);
  delay(100);

  countL = 0;
  int maxIndex = AllResi.length() - 1;

  for (int i = 0; i <= maxIndex; i++)
  {
    if (AllResi.charAt(i) == '.' || i == maxIndex)
    {
      countL++;
    }
  }
}

void readHistory()
{
  ///READ HISTORY
  File rhis = LittleFS.open("/history.txt", "r");
  if (!rhis) {
    //Read the rhis data and display it on LCD
    debugln("history.txt doesn't exist!");
    return;
  }
  History = rhis.readString();
  rhis.close();
  debugln();
  debug("History Content: ");
  debug(History);
  delay(100);
 countH = 0;
  int maxIndex = History.length() - 1;

  for (int i = 0; i <= maxIndex; i++)
  {
    if (History.charAt(i) == '.' || i == maxIndex)
    {
      countH++;
    }
  }
}



void addResi(int countL, String commands, String resi)
{
  if (countL < 10)
  {
    //Open the file
    File alis = LittleFS.open("/list.txt", "a+");

    //Write to the file
    alis.print(resi);
    delay(10);
    //Close the alis
    alis.close();

    debugln("Add successfuly");
    resi = resi.substring(0, resi.length() - 1);
    snprintf(msgi, MSG_BUFFER_SIZE, "%s - %s\nBerhasil terdaftar", commands.c_str(), resi.c_str());
    myBot.sendMessage(GroupID, msgi);
    delay(10);
    client.publish(pub_user.c_str(), "List Add");
  }
  else
  {
    String state = "List Full \nPlease Clear yout list to add new resi \nhttp://mybox-trial.web.app/";

    snprintf(msgi, MSG_BUFFER_SIZE, state.c_str());
    myBot.sendMessage(GroupID, msgi);
    delay(10);
    client.publish(pub_user.c_str(), "List Full");
  }
}

void addHis(int countH, String resi)
{
  if (countH < 10)
  {
    //Open the file
    File ahis = LittleFS.open("/history.txt", "a+");

    //Write to the file
    ahis.print(resi);
    delay(1);
    //Close the alis
    ahis.close();

    debugln("Add History successfuly");
    resi = resi.substring(0, resi.length() - 1);
    delay(10);

  }
  else
  {
    String state = "List Full \nPlease Clear yout list to add new resi \nhttp://mybox-trial.web.app/";

    snprintf(msgi, MSG_BUFFER_SIZE, state.c_str());
    myBot.sendMessage(GroupID, msgi);
    delay(10);
    client.publish(pub_user.c_str(), "List Full");
  }
}

void sendStatus(String listof,String NoResi) {
  
      String state = "Device ID : " + DevID + "/" + GroupID +
                 "\nStatus BOX : " + (isopen ? "Open" : "Close") +
                 "\nStatus Receive : " + (isreceive ? "Success" : "Not Yet");

  ////////// Telegram
  snprintf(msgi, MSG_BUFFER_SIZE, "%s\n%s:%s", state.c_str(),listof.c_str(), NoResi.c_str());
  myBot.sendMessage(GroupID, msgi);

  ///////// MQTT
  snprintf (msgi, MSG_BUFFER_SIZE, "List.%s", AllResi.c_str() );
  client.publish(pub_user.c_str(), msgi);
  snprintf (msgi, MSG_BUFFER_SIZE, "History.%s", History.c_str() );
  client.publish(pub_user.c_str(), msgi);
  delay(100);
}

void blinking() {
  for (int i = 0; i < 2; i++) {
    delay(300);
    digitalWrite(LED_BUILTIN, LOW);
    delay(100);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(100);
  }

  debugln("--Blinking LED--");
}

void bokuOpen(String commands) {
  isopen = true;
  //  unsigned long currentMillis = millis();
  //  lastTime = currentMillis; //open begin
  snprintf(msgi, MSG_BUFFER_SIZE, "OPEN");
  u8g2.clearBuffer();          // clear the internal memory
  u8g2.setFont(u8g2_font_logisoso28_tr);  // choose a suitable font at https://github.com/olikraus/u8g2/wiki/fntlistall
  u8g2.drawStr(8, 29, msgi); // write something to the internal memory
  u8g2.sendBuffer();         // transfer internal memory to the display
  debugln("Box Open");
  delay(10);
  servo.write(0);
  delay(200);

  ////////// Telegram
  if (commands.length() > 4)
  {
    snprintf(msgi, MSG_BUFFER_SIZE, "--- New Received ---\nID: ...%s", commands.c_str());
    myBot.sendMessage(GroupID, msgi);
    delay(10);
  }
  client.publish(pub_user.c_str(), "{\"state\":\"Box Open\"}");
  client.publish(pub_courier.c_str(), "{\"state\":\"Box Open\"}");
}

void bokuClose() {
  isopen = false;
  snprintf(msgi, MSG_BUFFER_SIZE, "CLOSE");
  u8g2.clearBuffer();          // clear the internal memory
  u8g2.setFont(u8g2_font_logisoso28_tr);  // choose a suitable font at https://github.com/olikraus/u8g2/wiki/fntlistall
  u8g2.drawStr(8, 29, msgi); // write something to the internal memory
  u8g2.sendBuffer();         // transfer internal memory to the display

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

void screen(String txt) {
  snprintf(msgi, MSG_BUFFER_SIZE, txt.c_str());
  debugln("show " + txt);
  u8g2.clearBuffer();          // clear the internal memory
  u8g2.setFont(u8g2_font_logisoso28_tr);  // choose a suitable font at https://github.com/olikraus/u8g2/wiki/fntlistall
  u8g2.drawStr(3, 29, msgi); // write something to the internal memory
  u8g2.sendBuffer();         // transfer internal memory to the display
  delay(10);
}
