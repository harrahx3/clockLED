// myStripLed.h
#ifndef MYSTRIPLED_H // include guard
#define MYSTRIPLED_H

#include "networkManager.h"

class NetworkManager;

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

void capacitiveTouchCallback();

class MyStripLed
{
public:
    enum class Mode
    {
        off,
        goAround,
        showTime,
        showPalette,
        showWeather,
        onFire,
        controller,
        analog,
        full,
        party
    };
    const static uint8_t nbOfMode = 10;

    MyStripLed();
    void printLocalTime(); // Hours=Red, Minutes=Green
    void fillLEDsFromPaletteColors(uint8_t colorIndex);
    void changePalette(uint8_t secondHand);
    void checkIR();
    void setMode(Mode p_mode)
    {
        this->mode = p_mode;
    }
    void setHoursColor(CRGB p_color)
    {
        hoursLedsColor = p_color;
    }
    void updateSun();
    void setLedsTunring()
    {
        fill_solid(leds, NUM_LEDS, CRGB::Black);
        for (uint8_t i = 0; i < NUM_LEDS; i += 5)
        {
            leds[(i + 32) % 60] = CRGB::White;
        }
        mode = Mode::goAround;
    }
    void update(NetworkManager *ntwmng);
    void goAround();
    void controller();
    void party();
    void nextPattern()
    {
        // add one to the current pattern number, and wrap around at the end
        // gCurrentPatternNumber = (gCurrentPatternNumber + 1) % ARRAY_SIZE(gPatterns);
    }

    void rainbow()
    {
        // FastLED's built-in rainbow generator
        fill_rainbow(leds, NUM_LEDS, gHue, 7);
    }

    void rainbowWithGlitter()
    {
        // built-in FastLED rainbow, plus some random sparkly glitter
        rainbow();
        addGlitter(80);
    }

    void addGlitter(fract8 chanceOfGlitter)
    {
        if (random8() < chanceOfGlitter)
        {
            leds[random16(NUM_LEDS)] += CRGB::White;
        }
    }

    void confetti()
    {
        // random colored speckles that blink in and fade smoothly
        fadeToBlackBy(leds, NUM_LEDS, 10);
        int pos = random16(NUM_LEDS);
        leds[pos] += CHSV(gHue + random8(64), 200, 255);
    }

    void sinelon()
    {
        // a colored dot sweeping back and forth, with fading trails
        fadeToBlackBy(leds, NUM_LEDS, 20);
        int pos = beatsin16(13, 0, NUM_LEDS - 1);
        leds[pos] += CHSV(gHue, 255, 192);
    }

    void bpm()
    {
        // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
        uint8_t BeatsPerMinute = 62;
        CRGBPalette16 palette = PartyColors_p;
        uint8_t beat = beatsin8(BeatsPerMinute, 64, 255);
        for (int i = 0; i < NUM_LEDS; i++)
        { //9948
            leds[i] = ColorFromPalette(palette, gHue + (i * 2), beat - gHue + (i * 10));
        }
    }

    void juggle()
    {
        // eight colored dots, weaving in and out of sync with each other
        fadeToBlackBy(leds, NUM_LEDS, 20);
        uint8_t dothue = 0;
        for (int i = 0; i < 8; i++)
        {
            leds[beatsin16(i + 7, 0, NUM_LEDS - 1)] |= CHSV(dothue, 200, 255);
            dothue += 32;
        }
    }
    void analog()
    {
        // Serial.println("analog");
        fill_solid(leds, NUM_LEDS, CRGB::Black);
        float val = analogRead(POTENTIOMETER_PIN) / MAX_ANALOG;
        //Serial.println(val);
        for (int i = 0; i < val * NUM_LEDS + 1; i++)
        {
            leds[i] = CRGB::White;
        }
        FastLED.show();
    }

    void setBrightness(unsigned int newB)
    {
        newB = newB < 0 ? 0 : newB;
        newB = newB > 255 ? 255 : newB;
        brightness = newB;
    }

    unsigned int readPotentiometer()
    {
        unsigned int val = analogRead(POTENTIOMETER_PIN);
        // Serial.println("sensor: ");
        // Serial.println(val);
        //setBrightness(val);
        return val;
    }

    void simulateFire();
    void changeColorWeather(NetworkManager *ntwmng);
    void Fire2012();
    CRGB HeatColor(uint8_t temperature);

private:
    Mode mode = Mode::full;

    // List of patterns to cycle through.  Each is defined as a separate function below.
    // typedef void (MyStripLed::*SimplePatternList[])();
    //SimplePatternList gPatterns = {rainbow, rainbowWithGlitter, confetti, sinelon, juggle, bpm};

    CRGBPalette16 currentPalette;
    TBlendType currentBlending;

    int selectedLed = 0;

    const static uint8_t NUM_LEDS = 60;   // How many leds in your strip?
    const static uint8_t OFFSET_LED = 32; // The top led

    uint8_t brightness = 3;                      // brightness for palettes
    const static uint8_t DATA_PIN = 25;          // ESP32 pin for LED control
    const static uint8_t POTENTIOMETER_PIN = 34; // ESP32 pin for Potentiometer -> brightness
    const static uint8_t IR_RECEIVE_PIN = 15;    // ESP32 pin for IR
    const static uint8_t CAPACITIVE_TOUCH_PIN = 4;    // ESP32 pin for capacitive touch sensor
    CRGB leds[NUM_LEDS];                         // list des couleurs de chaque led

    const static uint8_t capacitiveThreshold = 30;

    CRGB hoursLedsColor = CRGB::Red;
    CRGB minutesLedsColor = CRGB::Green;
    CRGB secondsLedsColor = CRGB::Blue;

    uint8_t startIndex = 0;
    int motionSpeed = -1;

    float MAX_ANALOG = 4095;

    uint8_t gCurrentPatternNumber = 0; // Index number of which pattern is current
    uint8_t gHue = 0;                  // rotating "base color" used by many of the patterns

    // There are two main parameters you can play with to control the look and
    // feel of your fire: COOLING (used in step 1 above), and SPARKING (used
    // in step 3 above).
    //
    // COOLING: How much does the air cool as it rises?
    // Less cooling = taller flames.  More cooling = shorter flames.
    // Default 55, suggested range 20-100
    const uint8_t COOLING = 55;

    // SPARKING: What chance (out of 255) is there that a new spark will be lit?
    // Higher chance = more roaring fire.  Lower chance = more flickery fire.
    // Default 120, suggested range 50-200.
    const uint8_t SPARKING = 120;
};

#endif /* MYSTRIPLED_H */
