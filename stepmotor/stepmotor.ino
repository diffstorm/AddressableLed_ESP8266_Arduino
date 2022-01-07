#include <ESP8266WiFi.h>
#include <FS.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string>
extern "C" {
#include "user_interface.h"
}
#include <Ticker.h>
#include "WebInterface.h"

#define WEB_CHECK_PERIOD 200 //[ms]

//------------------
bool web = false;
Ticker WebTimer;
void DoWeb()
{
  web = true;
}
inline void SetupWeb()
{
  WebTimer.attach_ms(WEB_CHECK_PERIOD, DoWeb);
}
//------------------
bool refresh = false;
Ticker FPSTimer;
void DoFPS()
{
  refresh = true;
}
void SetupFPS()
{
  extern unsigned int Frequency;
  FPSTimer.attach_ms(1000 / Frequency, DoFPS);
}

//------------------

void setup()
{
  ESP.wdtDisable();
  ESP.wdtEnable(WDTO_4S);
  SetupBoard();
  setupWiFi();
  setupWebServer();
  SetupFPS();
  SetupWeb();
}

void loop()
{
  ESP.wdtFeed();

  if (web)
  {
    web = false;
    server.handleClient();
  }
  
  if (refresh)
  {
    refresh = false;
    HandleStepMotor();
  }
}
