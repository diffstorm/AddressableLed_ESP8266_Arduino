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
#define NUM_LEDS 144
#define FRAMES_PER_SECOND 30

#define EFFECT_DURATION 20 //[sec]
#define WEB_CHECK_PERIOD 200 //[ms]

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

// Generic effects
//------------------
void LED_Effect_SweepingTurnSignal()
{
  static uint8_t inx = 0;
  const uint8_t lmax = 20;

  if(inx >= lmax)
  {
    while(inx)
    {
      LED_Clear(inx);
      inx--;
    }
    LED_Clear(0);
    LED_Push();
    delay(500);
  }
  else
  {
    LED_Set(inx, 255, 126, 0);
    inx++;
    LED_Push();
    //delay(50);
  }
}
//------------------

void PerformEffect()
{
  LED_Effect_SweepingTurnSignal();
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

void setup()
{
  ESP.wdtDisable();
  ESP.wdtEnable(WDTO_4S);
  setupWiFi();
  setupWebServer();
  LED_Init();
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
    PerformEffect();
  }
}
