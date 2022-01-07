#include "WebInterface.h"

const char* ssid = "AddrLed";

ESP8266WebServer server(80);

#include <FS.h>
#include <ESP8266HTTPUpdateServer.h>
ESP8266HTTPUpdateServer httpUpdater;

void setupWiFi(void)
{
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid);
}

void setupWebServer(void)
{
  httpUpdater.setup(&server);
  server.begin();
}
