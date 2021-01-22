#include <Arduino.h>
#include <WiFiManager.h>
#include "time.h"
#include <FastLED.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>
#include "myStripLed.h"
#include "global.h"

MyStripLed::MyStripLed() // Constructor
{
    //  this->mode=1;
    //this->mode = showPalette;
    this->selectedLed = 0;
    //this->showPalette = true;
    //this->showTime = false;

    FastLED.addLeds<WS2812B, MyStripLed::DATA_PIN, GRB>(leds, MyStripLed::NUM_LEDS);
    // FastLED.addLeds<WS2812B, this->dataPin, GRB>(leds, MyStripLed::NUM_LEDS);

    FastLED.setBrightness(this->brightness);

    this->currentPalette = RainbowColors_p;
    this->currentBlending = LINEARBLEND;
}

void MyStripLed::printLocalTime()
{
    for (int i = 0; i < MyStripLed::NUM_LEDS; i++)
    {
        leds[i] = CRGB::Black;
    }

    struct tm timeinfo;
    if (!getLocalTime(&timeinfo))
    {
        Serial.println("Failed to obtain time");
        return;
    }
    Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
    Serial.print("Day of week: ");
    Serial.println(&timeinfo, "%A");
    Serial.print("Month: ");
    Serial.println(&timeinfo, "%B");
    Serial.print("Day of Month: ");
    Serial.println(&timeinfo, "%d");
    Serial.print("Year: ");
    Serial.println(&timeinfo, "%Y");
    Serial.print("Hour: ");
    Serial.println(&timeinfo, "%H");
    Serial.print("Hour (12 hour format): ");
    Serial.println(&timeinfo, "%I");
    Serial.print("Minute: ");
    Serial.println(&timeinfo, "%M");
    Serial.print("Second: ");
    Serial.println(&timeinfo, "%S");

    /*Serial.println("Time variables");
    char timeHour[3];
    strftime(timeHour, 3, "%H", &timeinfo);
    Serial.println(timeHour);
    char timeWeekDay[10];
    strftime(timeWeekDay, 10, "%A", &timeinfo);
    Serial.println(timeWeekDay);
    Serial.println();*/

    //this->leds[0] = CRGB::Blue;
    //FastLED.show();

    char timeHour12_char[3];
    strftime(timeHour12_char, 3, "%I", &timeinfo);
    Serial.println(timeHour12_char);
    char timeMinute12_char[3];
    strftime(timeMinute12_char, 3, "%M", &timeinfo);
    Serial.println(timeMinute12_char);
    char timeSecond_char[3];
    strftime(timeSecond_char, 3, "%S", &timeinfo);
    Serial.println(timeSecond_char);
    unsigned int timeHour12 = std::atoi(timeHour12_char);
    int timeMinute12 = std::atoi(timeMinute12_char);
    unsigned int timeSecond = std::atoi(timeSecond_char);

    timeHour12 %= 12;
    timeMinute12 %= 60;
    timeSecond %= 60;

    int hoursLed = (timeHour12 * 5 * 60 + timeMinute12 * 5) / 60;
    Serial.println(hoursLed);
    //this->leds[hoursLed] = CRGB::Red;
    for (int i = 0; i <= hoursLed; i++)
    {
        leds[(i + 30) % 60] = hoursLedsColor;
    }

    int secondsLed = timeSecond;
    Serial.println(secondsLed);
    this->leds[(secondsLed + 30) % 60] = secondsLedsColor;

    int minutesLed = timeMinute12; // * 12 / 60;
    Serial.println(minutesLed);
    this->leds[(minutesLed + 30) % 60] = minutesLedsColor;

    for (int i = 0; i <= MyStripLed::NUM_LEDS; i++)
    {
        if (i != hoursLed && i != minutesLed && i != secondsLed)
        {
            leds[(i + 30) % 60] %= 1;
        }
        else
        {
            leds[(i + 30) % 60] %= 100;
        }
    }

    FastLED.show();
}

void MyStripLed::fillLEDsFromPaletteColors(uint8_t colorIndex)
{
    //uint8_t brightness = this->brightness;

    for (int i = 0; i < MyStripLed::NUM_LEDS; i++)
    {
        leds[i] = ColorFromPalette(currentPalette, colorIndex, this->brightness, currentBlending);
        colorIndex += 3;
    }
}

void MyStripLed::changePalette(uint8_t secondHand)
{
    if (secondHand == 0)
    {
        currentPalette = RainbowColors_p;
        currentBlending = LINEARBLEND;
    }
    if (secondHand == 10)
    {
        currentPalette = RainbowStripeColors_p;
        currentBlending = NOBLEND;
    }
    if (secondHand == 15)
    {
        currentPalette = RainbowStripeColors_p;
        currentBlending = LINEARBLEND;
    }
    if (secondHand == 20)
    {
        currentPalette = OceanColors_p;
        currentBlending = LINEARBLEND;
    }
    if (secondHand == 25)
    {
        currentPalette = CloudColors_p;
        currentBlending = LINEARBLEND;
    }
    if (secondHand == 30)
    {
        currentPalette = ForestColors_p;
        currentBlending = NOBLEND;
    }
    if (secondHand == 35)
    {
        currentPalette = LavaColors_p;
        currentBlending = LINEARBLEND;
    }
    if (secondHand == 40)
    {
        currentPalette = CloudColors_p;
        currentBlending = LINEARBLEND;
    }
    if (secondHand == 45)
    {
        currentPalette = PartyColors_p;
        currentBlending = LINEARBLEND;
    }

    /* if (secondHand == 50)
    {
        currentPalette = myRedWhiteBluePalette_p;
        currentBlending = NOBLEND;
    }
    if (secondHand == 55)
    {
        currentPalette = myRedWhiteBluePalette_p;
        currentBlending = LINEARBLEND;
    }*/
}

void MyStripLed::update()
{

    if (this->mode == showTime)
    {
        this->printLocalTime();
    }

    if (this->mode == showPalette)
    {
        startIndex += motionSpeed; /* motion speed */
        this->fillLEDsFromPaletteColors(startIndex);
    }
}