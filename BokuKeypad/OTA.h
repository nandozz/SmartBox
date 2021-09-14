#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

void OTAfunc(const char *ssid,const char *password,const char *host)
{
//  Serial.begin(115200);
  debugln("OTA Func is Called");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  delay(100);
  while (WiFi.waitForConnectResult() != WL_CONNECTED)
  {
    debugln("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }

  ArduinoOTA.setHostname(host);
  ArduinoOTA.onStart([]()
                     {
                       String type;
                       if (ArduinoOTA.getCommand() == U_FLASH)
                       {
                         type = "sketch";
                       }
                       else
                       { // U_FS
                         type = "filesystem";
                       }

                       // NOTE: if updating FS this would be the place to unmount FS using FS.end()
                       debugln("Start updating " + type);
                     });
  ArduinoOTA.onEnd([]()
                   { debugln("\nEnd"); });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total)
                        { Serial.printf("Progress: %u%%\r", (progress / (total / 100))); });
  ArduinoOTA.onError([](ota_error_t error)
                     {
                       Serial.printf("Error[%u]: ", error);
                       if (error == OTA_AUTH_ERROR)
                       {
                         debugln("Auth Failed");
                       }
                       else if (error == OTA_BEGIN_ERROR)
                       {
                         debugln("Begin Failed");
                       }
                       else if (error == OTA_CONNECT_ERROR)
                       {
                         debugln("Connect Failed");
                       }
                       else if (error == OTA_RECEIVE_ERROR)
                       {
                         debugln("Receive Failed");
                       }
                       else if (error == OTA_END_ERROR)
                       {
                         debugln("End Failed");
                       }
                     });
  ArduinoOTA.begin();
  debugln("Ready");
  debug("IP OTA address: ");
  debugln(WiFi.localIP());
}
