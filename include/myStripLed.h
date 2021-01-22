// myStripLed.h
#ifndef MYSTRIPLED_H // include guard
#define MYSTRIPLED_H

#include <Arduino.h>
#include <WiFiManager.h>
#include "time.h"
#include <FastLED.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>
#include "global.h"

class MyStripLed
{
public:
    MyStripLed();
    void printLocalTime(); // Hours=Red, Minutes=Green
    void fillLEDsFromPaletteColors(uint8_t colorIndex);
    void changePalette(uint8_t secondHand);
    void setMode(uint8_t p_mode)
    {
        switch (p_mode)
        {
        case 0:
            this->mode = showTime;
            break;
        case 1:
            this->mode = showPalette;
            break;

        default:
            break;
        }
    }
    void setHoursColor(CRGB p_color)
    {
        hoursLedsColor = p_color;
    }
    void update();

    enum mode
    {
        showTime,
        showPalette
    };

private:
    int mode = showTime;

    CRGBPalette16 currentPalette;
    TBlendType currentBlending;

    int selectedLed = 0;

    const static unsigned int NUM_LEDS = 60; // How many leds in your strip?
    unsigned int brightness = 3;
    const static unsigned int DATA_PIN = 25; // ESP32 pin

    CRGB leds[NUM_LEDS];

    CRGB hoursLedsColor = CRGB::Red;
    CRGB minutesLedsColor = CRGB::Green;
    CRGB secondsLedsColor = CRGB::Blue;

    uint8_t startIndex = 0;
    uint8_t motionSpeed = -1;
};

#endif /* MYSTRIPLED_H */
