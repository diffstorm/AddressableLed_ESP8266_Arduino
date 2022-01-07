#include "WebInterface.h"

const char* ssid = "Motor";

ESP8266WebServer server(80);

#include <FS.h>
#include <ESP8266HTTPUpdateServer.h>
ESP8266HTTPUpdateServer httpUpdater;

String page = "<html><body>" \ 
              "<p><a href=\"?d=0&f=200&s=200\"><button>FASTEST-LEFT</button></a></p>" \
              "<p><a href=\"?d=1&f=200&s=200\"><button>FASTEST-RIGHT</button></a></p>" \
              "<p><a href=\"?d=0&f=120&s=150\"><button>FASTER-LEFT</button></a></p>" \
              "<p><a href=\"?d=1&f=120&s=150\"><button>FASTER-RIGHT</button></a></p>" \
              "<p><a href=\"?d=0&f=60&s=100\"><button>FAST-LEFT</button></a></p>" \
              "<p><a href=\"?d=1&f=60&s=100\"><button>FAST-RIGHT</button></a></p>" \
              "<p><a href=\"?d=0&f=30&s=80\"><button>SLOW-LEFT</button></a></p>" \ 
              "<p><a href=\"?d=1&f=30&s=80\"><button>SLOW-RIGHT</button></a></p>" \
              "<p><a href=\"?d=0&f=10&s=50\"><button>SLOWER-LEFT</button></a></p>" \ 
              "<p><a href=\"?d=1&f=10&s=50\"><button>SLOWER-RIGHT</button></a></p>" \
              "<p><a href=\"?d=0&f=3&s=10\"><button>SLOWEST-LEFT</button></a></p>" \ 
              "<p><a href=\"?d=1&f=3&s=10\"><button>SLOWEST-RIGHT</button></a></p>" \
              "</body></html>";

void setupWiFi(void)
{
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid);
}

void setupWebServer(void)
{
  server.on("/", []()
  {
    String DIR = server.arg("d");
    String FREQ = server.arg("f");
    String STEPS = server.arg("s");

    if(DIR.length() > 0)
    {
      SetDirection(DIR.toInt() > 0 ? 1 : 0);
    }

    if(FREQ.length() > 0)
    {
      SetFrequency(FREQ.toInt());
    }

    if(STEPS.length() > 0)
    {
      SetSteps(STEPS.toInt());
    }
    
    server.send(200, "text/html", page);
  });

  server.on("/reset", []()
  {
    server.send(200, "text/plain", "OK");
    delay(500);
    ESP.restart();
  });
  
  httpUpdater.setup(&server);
  server.begin();
}
