#include <Arduino.h>
#include <WiFiManager.h>
#include "time.h"
#include <FastLED.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include "networkManager.h"
#include "myStripLed.h"

// How many leds in your strip?
//#define NUM_LEDS 60
//#define BRIGHTNESS 30
//#define DATA_PIN 25

extern CRGBPalette16 myRedWhiteBluePalette;
extern const TProgmemPalette16 myRedWhiteBluePalette_p PROGMEM;

MyStripLed myStripLed;
NetworkManager ntwmng (&myStripLed);

// For led chips like WS2812, which have a data line, ground, and power, you just
// need to define DATA_PIN.  For led chipsets that are SPI based (four wires - data, clock,
// ground, and power), like the LPD8806 define both DATA_PIN and CLOCK_PIN
// Clock pin only needed for SPI based chipsets when not using hardware SPI

//const char* ssid     = "SFR_EBEF";
//const char* password = "c6kphxmvqdhsflvgwaj7";

void setup()
{
  Serial.begin(115200);

  ntwmng.setWifi();
  ntwmng.setSPIFFS();
  ntwmng.setServer();

  delay(5000);

  //changeColorWeather();
}

void loop()
{
  //delay(1000);
  myStripLed.update(&ntwmng);
  //FastLED.show();
  FastLED.delay(1000 / 10);
}
