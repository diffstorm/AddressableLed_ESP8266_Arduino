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
#define NUM_LEDS 24//114
#define FRAMES_PER_SECOND 20

#define EFFECT_DURATION 30 //[sec]
#define WEB_CHECK_PERIOD 200 //[ms]

//#define method_NeoPixelBus
//#define method_Adafruit_NeoPixel
#define method_FastLED

uint8_t gHue = 0;

#ifdef method_NeoPixelBus
#include <NeoPixelBus.h>
#define FASTLED_ESP8266_RAW_PIN_ORDER
#define FASTLED_ALLOW_INTERRUPTS 0
#include <FastLED.h>

NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> strip(NUM_LEDS, LED_PIN);

void LED_Init()
{
  strip.Begin();
  strip.Show();
}

void LED_Clear(int i)
{
  RgbColor pixel;
  pixel = RgbColor(0, 0, 0);
  strip.SetPixelColor(i, pixel);
}

void LED_Set(int i, int r, int g, int b)
{
  RgbColor pixel;
  pixel = RgbColor(r, g, b);
  strip.SetPixelColor(i, pixel);
}

void LED_Push()
{
  strip.Show();
}
#endif

#ifdef method_Adafruit_NeoPixel
#include <Adafruit_NeoPixel.h>

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);

void LED_Init()
{
  strip.begin();
  strip.show();
}

void LED_Clear(int i)
{
  strip.setPixelColor(i, 0, 0, 0);
}

void LED_Set(int i, int r, int g, int b)
{
  strip.setPixelColor(i, r, g, b);
}

void LED_Push()
{
  strip.show();
}
#endif

#ifdef method_FastLED
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

// Lib Specific effects
//----------------------
void LED_Effect_Rainbow() // FastLED's built-in rainbow generator
{
  fill_rainbow(leds, NUM_LEDS, gHue, 7);
}
//---
void addGlitter( fract8 chanceOfGlitter)
{
  if (random8() < chanceOfGlitter)
  {
    leds[random16(NUM_LEDS)] += CRGB::White;
  }
}

void LED_Effect_RainbowWithGlitter() // built-in FastLED rainbow, plus some random sparkly glitter
{
  LED_Effect_Rainbow();
  addGlitter(80);
}
//---
void LED_Effect_Confetti() // random colored speckles that blink in and fade smoothly
{
  fadeToBlackBy(leds, NUM_LEDS, 10);
  int pos = random16(NUM_LEDS);
  leds[pos] += CHSV( gHue + random8(64), 200, 255);
}
//---
void LED_Effect_Sinelon() // a colored dot sweeping back and forth, with fading trails
{
  fadeToBlackBy( leds, NUM_LEDS, 20);
  int pos = beatsin16(13, 0, NUM_LEDS);
  leds[pos] += CHSV( gHue, 255, 192);
}
//---
void LED_Effect_BeatsPerMinute() // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
{
  uint8_t BeatsPerMinute = FRAMES_PER_SECOND;
  CRGBPalette16 palette = PartyColors_p;
  uint8_t beat = beatsin8( BeatsPerMinute, 64, 255);
  for ( int i = 0; i < NUM_LEDS; i++)
  {
    leds[i] = ColorFromPalette(palette, gHue + (i * 2), beat - gHue + (i * 10));
  }
}
//---
void LED_Effect_Juggle() // eight colored dots, weaving in and out of sync with each other
{
  fadeToBlackBy(leds, NUM_LEDS, 20);
  byte dothue = 0;
  for (int i = 0; i < 8; i++)
  {
    leds[beatsin16(i + 7, 0, NUM_LEDS)] |= CHSV(dothue, 200, 255);
    dothue += 32;
  }
}
//---
void LED_Effect_Chase()
{
  static uint8_t i = 0;

  if (i >= 3)
    leds[i - 3] = CHSV( 224, 255, 0);
  if (i >= 2)
    leds[i - 2] = CHSV( 224, 255, 50);
  if (i >= 1)
    leds[i - 1] = CHSV( 224, 255, 150);
  leds[i + 0] = CHSV( 224, 255, 255);
  if (i + 1 < NUM_LEDS - 1)
    leds[i + 1] = CHSV( 224, 255, 150);
  if (i + 2 < NUM_LEDS - 1)
    leds[i + 2] = CHSV( 224, 255, 50);
  if (i + 3 < NUM_LEDS - 1)
    leds[i + 3] = CHSV( 224, 255, 0);
  i++;
  if (i == NUM_LEDS)
  {
    i = 0;
  }
}
#endif

// Generic effects
//------------------
void LED_Effect_WalkingLed()
{
  static uint8_t inx = 0;

  if (inx == 0)
    LED_Clear(NUM_LEDS - 1);
  else
    LED_Clear(inx - 1);

  LED_Set(inx, 0, 0, 255);

  inx++;
  if (inx >= NUM_LEDS)
    inx = 0;
}
//---
void LED_Effect_Punkah()
{
  static uint8_t state = 0;
  static uint8_t inx = 0;
  static uint8_t interval = 0;
  static uint8_t color = 0;
  const uint32_t colors[] = {0xFF0000, 0x00FF00, 0x0000FF, 0xFFFF00, 0x00FFFF, 0xFF00FF};

  switch(state)
  {
    case 0: // set next led color
      LED_Set(inx, (colors[color] >> 16) & 0xFF, (colors[color] >> 8) & 0xFF, colors[color] & 0xFF);
      interval = 0;
      state++;
      break;
    case 1: // delay between 2 leds. Total effect time is 1500ms
      interval++;
      if(interval >= ((1500 / NUM_LEDS) / (1000 / FRAMES_PER_SECOND) + 1))
      {
        state++;
      }
      break;
    case 2: // check for last led
      inx++;
      if (inx >= NUM_LEDS)
      {
        inx = 0;
        interval = 0;
        color++;
        if(color >= sizeof(colors))
          color = 0;
        state++;
      }
      else
        state = 0;
      break;
    case 3: // 500 ms wait at the end
      interval++;
      if(interval >= (500 / (1000 / FRAMES_PER_SECOND)))
      {
        state = 0;
      }
      break;
  }
}
//---
void LED_Effect_Dim()
{
  static uint8_t dim = 0;
  static bool dir = false;
  
  if(dir == false && dim == 255)
  {
    dir = true;
  }
  else if(dir != false && dim == 0)
  {
    dir = false;
  }

  if(dir == false)
    dim++;
  else
    dim--;
  
  fill_rainbow(leds, NUM_LEDS, gHue, 7);
  FastLED.setBrightness(dim);
}
//------------------

typedef void (*List[])();

List EffectList = {
  LED_Effect_Dim,
  LED_Effect_Punkah,
#ifdef method_FastLED
  LED_Effect_Rainbow,
  LED_Effect_RainbowWithGlitter,
  LED_Effect_Confetti,
  LED_Effect_Sinelon,
  LED_Effect_BeatsPerMinute,
  LED_Effect_Juggle,
  LED_Effect_Chase,
#endif
  LED_Effect_WalkingLed
};

uint8_t EffectN = 0; // Index number of which pattern is current

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

void NextEffect()
{
  EffectN = (EffectN + 1) % ARRAY_SIZE(EffectList);
}

void PerformEffect()
{
  gHue += 3;
  EffectList[EffectN]();
  LED_Push();
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
Ticker NextEffectTimer;
inline void SetupNextEffect()
{
  NextEffectTimer.attach_ms(1000 * EFFECT_DURATION, NextEffect);
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
  SetupNextEffect();
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
