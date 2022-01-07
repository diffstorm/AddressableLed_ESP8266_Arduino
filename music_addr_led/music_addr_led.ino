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

#define LED_PIN 3
#define NUM_LEDS 10

#define WEB_CHECK_PERIOD 200 //[ms]
#define FRAMES_PER_SECOND 5

#define FASTLED_ESP8266_RAW_PIN_ORDER
#define FASTLED_ALLOW_INTERRUPTS 0
#include <FastLED.h>

#define COLOR_ORDER GRB
#define CHIPSET     WS2812B

CRGB leds[NUM_LEDS];

void LED_Init()
{
  FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
}

void LED_Clear(int i)
{
  leds[i].r = 0;
  leds[i].g = 0;
  leds[i].b = 0;
}

void LED_Set(int i, int r, int g, int b)
{
  leds[i].r = r;
  leds[i].g = g;
  leds[i].b = b;
}

void LED_Push()
{
  FastLED.show();
}

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
inline void SetupFPS()
{
  FPSTimer.attach_ms(1000 / FRAMES_PER_SECOND, DoFPS);
}
//------------------
void PerformEffect()
{
  for(int i = 0; i < NUM_LEDS; i++)
  {
    LED_Set(i, random(255), random(255), random(255));
  }
  LED_Push();
}

void setup()
{
  ESP.wdtDisable();
  ESP.wdtEnable(WDTO_4S);
  setupWiFi();
  setupWebServer();
  LED_Init();
  SetupFPS();
  SetupWeb();
  randomSeed(128);
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
    PerformEffect();
  }
}
