#include "WebInterface.h"

const char* ssid = "<ssid>";
const char* password = "<password>";

ESP8266WebServer server(80);

#include <FS.h>
#include <ESP8266HTTPUpdateServer.h>
ESP8266HTTPUpdateServer httpUpdater;

void setupWiFi(void)
{
	WiFi.begin(ssid, password);
	while (WiFi.status() != WL_CONNECTED)
	{
		delay(500);
	}
}

void setupWebServer(void)
{
  httpUpdater.setup(&server);
  server.begin();
}
