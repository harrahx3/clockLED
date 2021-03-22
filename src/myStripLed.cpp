#include <Arduino.h>
#include <WiFiManager.h>
#include "time.h"
#include <FastLED.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <stdio.h>  /* printf, scanf, puts, NULL */
#include <stdlib.h> /* srand, rand */
#include <time.h>   /* time */
#include "networkManager.h"
#include "myStripLed.h"
//#include "global.h"

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
    //setLedsTunring();
    setMode(Mode::showTime);
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
        //Serial.println("Failed to obtain time");
        return;
    }

    char timeHour12_char[3];
    //Serial.println(timeinfo.tm_mon);

    strftime(timeHour12_char, 3, "%I", &timeinfo);
    // Serial.println(timeHour12_char);
    char timeMinute12_char[3];
    strftime(timeMinute12_char, 3, "%M", &timeinfo);
    // Serial.println(timeMinute12_char);
    char timeSecond_char[3];
    strftime(timeSecond_char, 3, "%S", &timeinfo);
    // Serial.println(timeSecond_char);
    unsigned int timeHour12 = std::atoi(timeHour12_char);
    int timeMinute12 = std::atoi(timeMinute12_char);
    unsigned int timeSecond = std::atoi(timeSecond_char);

    timeHour12 %= 12;
    timeMinute12 %= 60;
    timeSecond %= 60;

    int hoursLed = (timeHour12 * 5 * 60 + timeMinute12 * 5) / 60;
    // hoursLed = 1+readPotentiometer()*60/MAX_ANALOG;
    // Serial.println(hoursLed);
    //this->leds[hoursLed] = CRGB::Red;
    for (int i = 0; i <= hoursLed; i++)
    {
        leds[(i + OFFSET_LED) % NUM_LEDS] = hoursLedsColor;
    }

    unsigned int monthLed = timeinfo.tm_mon * 5;
    leds[(monthLed + OFFSET_LED) % NUM_LEDS] = CRGB::White;
    unsigned int dayLed = timeinfo.tm_mday;
    leds[(dayLed + OFFSET_LED) % NUM_LEDS] = CRGB::White;

    int secondsLed = timeSecond;
    // Serial.println(secondsLed);
    this->leds[(secondsLed + OFFSET_LED) % NUM_LEDS] = secondsLedsColor;

    int minutesLed = timeMinute12; // * 12 / 60;
    // Serial.println(minutesLed);
    this->leds[(minutesLed + OFFSET_LED) % NUM_LEDS] = minutesLedsColor;

    for (int i = 0; i <= MyStripLed::NUM_LEDS; i++)
    {
        if (i != hoursLed && i != minutesLed && i != secondsLed && i != monthLed && i != dayLed)
        {
            leds[(i + OFFSET_LED) % NUM_LEDS] %= 1;
        }
        else
        {
            leds[(i + OFFSET_LED) % NUM_LEDS] %= 100;
        }
    }

    FastLED.show();
}

void MyStripLed::fillLEDsFromPaletteColors(uint8_t colorIndex)
{
    //uint8_t brightness = this->brightness;

    setBrightness(readPotentiometer() * 255 / MAX_ANALOG);

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

void MyStripLed::Fire2012()
{
    // Array of temperature readings at each simulation cell
    static byte heat[NUM_LEDS];

    // Step 1.  Cool down every cell a little
    for (int i = 0; i < NUM_LEDS; i++)
    {
        heat[i] = qsub8(heat[i], random8(0, ((COOLING * 10) / NUM_LEDS) + 2));
    }

    // Step 2.  Heat from each cell drifts 'up' and diffuses a little
    for (int k = NUM_LEDS - 3; k > 0; k--)
    {
        heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2]) / 3;
    }

    // Step 3.  Randomly ignite new 'sparks' of heat near the bottom
    if (random8() < SPARKING)
    {
        int y = random8(7);
        heat[y] = qadd8(heat[y], random8(160, 255));
    }

    // Step 4.  Map from heat cells to LED colors
    for (int j = 0; j < NUM_LEDS; j++)
    {
        leds[j] = HeatColor(heat[j]);
    }
}

// CRGB HeatColor( uint8_t temperature)
// [to be included in the forthcoming FastLED v2.1]
//
// Approximates a 'black body radiation' spectrum for
// a given 'heat' level.  This is useful for animations of 'fire'.
// Heat is specified as an arbitrary scale from 0 (cool) to 255 (hot).
// This is NOT a chromatically correct 'black body radiation'
// spectrum, but it's surprisingly close, and it's extremely fast and small.
//
// On AVR/Arduino, this typically takes around 70 bytes of program memory,
// versus 768 bytes for a full 256-entry RGB lookup table.

CRGB MyStripLed::HeatColor(uint8_t temperature)
{
    CRGB heatcolor;

    // Scale 'heat' down from 0-255 to 0-191,
    // which can then be easily divided into three
    // equal 'thirds' of 64 units each.
    uint8_t t192 = scale8_video(temperature, 192);

    // calculate a value that ramps up from
    // zero to 255 in each 'third' of the scale.
    uint8_t heatramp = t192 & 0x3F; // 0..63
    heatramp <<= 2;                 // scale up to 0..252

    // now figure out which third of the spectrum we're in:
    if (t192 & 0x80)
    {
        // we're in the hottest third
        heatcolor.r = 255;      // full red
        heatcolor.g = 255;      // full green
        heatcolor.b = heatramp; // ramp up blue
    }
    else if (t192 & 0x40)
    {
        // we're in the middle third
        heatcolor.r = 255;      // full red
        heatcolor.g = heatramp; // ramp up green
        heatcolor.b = 0;        // no blue
    }
    else
    {
        // we're in the coolest third
        heatcolor.r = heatramp; // ramp up red
        heatcolor.g = 0;        // no green
        heatcolor.b = 0;        // no blue
    }

    return heatcolor;
}

void MyStripLed::simulateFire()
{
    // initialize random seed:
    srand(time(NULL));

    for (uint8_t i = 0; i < NUM_LEDS; i++)
    {
        int r = rand() % 50 + 205;
        int g = rand() % 5;
        int b = rand() % 1;
        leds[i] = CRGB(r, g, b);
    }
    // generate secret number between 1 and 10:
    //int iSecret = rand() % 10 + 1;

    FastLED.show();
    
    // Fire2012 by Mark Kriegsman, July 2012
    // as part of "Five Elements" shown here: http://youtu.be/knWiGsmgycY
    //
    // This basic one-dimensional 'fire' simulation works roughly as follows:
    // There's a underlying array of 'heat' cells, that model the temperature
    // at each point along the line.  Every cycle through the simulation,
    // four steps are performed:
    //  1) All cells cool down a little bit, losing heat to the air
    //  2) The heat from each cell drifts 'up' and diffuses a little
    //  3) Sometimes randomly new 'sparks' of heat are added at the bottom
    //  4) The heat from each cell is rendered as a color into the leds array
    //     The heat-to-color mapping uses a black-body radiation approximation.
    //
    // Temperature is in arbitrary units from 0 (cold black) to 255 (white hot).
    //
    // This simulation scales it self a bit depending on NUM_LEDS; it should look
    // "OK" on anywhere from 20 to 100 LEDs without too much tweaking.
    //
    // I recommend running this simulation at anywhere from 30-100 frames per second,
    // meaning an interframe delay of about 10-35 milliseconds.
    //
    //
}

void MyStripLed::changeColorWeather(NetworkManager *ntwmng)
{
    StaticJsonDocument<1000> doc = ntwmng->requestJsonApi();
    const char *name = doc["name"];
    //Serial.println(name);

    int tempFeel = doc["main"]["temp_min"];
    //Serial.println(tempFeel);

   /* int visibility = doc["visibility"];
    //Serial.println(visibility);

    int id2 = doc["id"];
    //Serial.println(String(id2));
    int cod = doc["cod"];
    //Serial.println(String(cod));

    int id = doc["id"];*/
    //  const char *description = doc["weather"][0]["description"];

    //Serial.println(String(id));
    // //Serial.println(description);

    //this->setMode(MyStripLed::Mode::showPalette);
   // uint8_t newPalette = 45;
    CRGB newColor = CRGB::Green;
   // const char *weather = doc["weather"][0]["main"];
    int weatherId = doc["weather"][0]["id"];
    //Serial.println(weatherId);
    //Serial.println(weather);
    //Serial.println(String(weatherId / 100));

    switch (int(weatherId / 100))
    {
    case 2:
    case 3:
    case 6:
    case 5:
    case 7:
       // newPalette = 25;
        newColor = CRGB::Red;
        break;

    case 8:
       // newPalette = 35;
        newColor = CRGB::Blue;
        break;

    default:
        break;
    }
    //Serial.println("feellike");
    //Serial.println(tempFeel);
    //Serial.println(newColor);
    for (uint8_t i = 0; i < tempFeel; i++)
    {
        leds[i] = newColor;
    }
    for (uint8_t i = tempFeel; i < NUM_LEDS; i++)
    {
        leds[i] = CRGB::Black;
    }
    FastLED.show();

    //this->changePalette(newPalette);
}

void MyStripLed::goAround()
{
    // CRGB leds_cop[NUM_LEDS] = leds;
    // if (motionSpeed < 0)
    //{
    //CRGB init_led = leds[0];
    for (uint8_t i = 0; i < NUM_LEDS; i++)
    {
        leds[i] = leds[(i - motionSpeed) % NUM_LEDS];
    }
    // leds[NUM_LEDS - 1] = init_led;
    //}
    // leds = leds_cop;

    FastLED.show();
}

void MyStripLed::controller()
{
    fill_solid(leds, NUM_LEDS, CRGB::Black);
    uint8_t x_pin = 34;
    uint8_t y_pin = 32;
    float x_val = 2 * (((analogRead(x_pin) - (2830 - 2048)) / MAX_ANALOG) - .5);
    float y_val = 2 * (((analogRead(y_pin) - (2830 - 2048)) / MAX_ANALOG) - .5);
    float dx = acos(x_val);
    float dy = asin(y_val);
    leds[(int((dx / PI * NUM_LEDS / 2)) + OFFSET_LED) % NUM_LEDS] = CRGB::Red;
    leds[(int((dy / PI * NUM_LEDS / 2)) + OFFSET_LED) % NUM_LEDS] = CRGB::Green;
    //Serial.print(analogRead(x_pin));
    //Serial.print("\t");
    //Serial.print(x_val);
    //Serial.print("\t");
    //Serial.print(dx);
    //Serial.print("\t");
    //Serial.print((int((dx / PI * NUM_LEDS / 2)) + OFFSET_LED) % NUM_LEDS);
    //Serial.println();
}

void MyStripLed::update(NetworkManager *ntwmng)
{
    switch (this->mode)
    {
    case Mode::off:
        fill_solid(leds, NUM_LEDS, CRGB::Black);
        /* for (uint8_t i = 0; i < NUM_LEDS; i++)
        {
            if (leds[i])
            {
                leds[i] = CRGB::Black;
                FastLED.show();
            }
        }*/
        FastLED.show();

        FastLED.delay(500);
        break;
    case Mode::showTime:
        this->printLocalTime();
        FastLED.delay(100);
        break;
    case Mode::showPalette:
        startIndex += motionSpeed;
        this->fillLEDsFromPaletteColors(startIndex);
        FastLED.delay(100);
        break;
    case Mode::showWeather:
        this->changeColorWeather(ntwmng);
        FastLED.delay(1000 * 5);
        break;
    case Mode::onFire:
        // Add entropy to random number generator; we use a lot of it.
        random16_add_entropy(random8());

        this->Fire2012(); // run simulation frame
        //this->simulateFire();
        FastLED.show();
        FastLED.delay(30);
        break;
    case Mode::goAround:
        goAround();
        FastLED.delay(100);
        break;
    case Mode::controller:
        controller();
        FastLED.delay(40);
        break;
    case Mode::analog:
        analog();
        FastLED.delay(50);
        break;
    default:
        break;
    }
}