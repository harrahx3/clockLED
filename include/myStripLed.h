// myStripLed.h
#ifndef MYSTRIPLED_H // include guard
#define MYSTRIPLED_H

#include "networkManager.h"

class NetworkManager;

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
        analog
    };
    MyStripLed();
    void printLocalTime(); // Hours=Red, Minutes=Green
    void fillLEDsFromPaletteColors(uint8_t colorIndex);
    void changePalette(uint8_t secondHand);
    void setMode(Mode p_mode)
    {
        this->mode = p_mode;
    }
    void setHoursColor(CRGB p_color)
    {
        hoursLedsColor = p_color;
    }
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
    void analog()
    {
        Serial.println("analog");
        fill_solid(leds, NUM_LEDS, CRGB::Black);
        float val = analogRead(POTENTIOMETER_PIN)/MAX_ANALOG;
        Serial.println(val);
        for (int i = 0; i < val*NUM_LEDS+1; i++)
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
    Mode mode = Mode::onFire;

    CRGBPalette16 currentPalette;
    TBlendType currentBlending;

    int selectedLed = 0;

    const static uint8_t NUM_LEDS = 60; // How many leds in your strip?
    const static uint8_t OFFSET_LED = 32; // The top led

    uint8_t brightness = 3;                           // brightness for palettes
    const static uint8_t DATA_PIN = 25;               // ESP32 pin for LED control
    const static unsigned int POTENTIOMETER_PIN = 34; // ESP32 pin for Potentiometer -> brightness

    CRGB leds[NUM_LEDS]; // list des couleurs de chaque led

    CRGB hoursLedsColor = CRGB::Red;
    CRGB minutesLedsColor = CRGB::Green;
    CRGB secondsLedsColor = CRGB::Blue;

    uint8_t startIndex = 0;
    int motionSpeed = -1;

    float MAX_ANALOG = 4095;

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
