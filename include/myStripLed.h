// myStripLed.h
#ifndef MYSTRIPLED_H // include guard
#define MYSTRIPLED_H

#include "networkManager.h"

class NetworkManager;

class MyStripLed
{
public:
    enum Mode
    {
        showTime,
        showPalette,
        showWeather,
        onFire
    };
    MyStripLed();
    void printLocalTime(); // Hours=Red, Minutes=Green
    void fillLEDsFromPaletteColors(uint8_t colorIndex);
    void changePalette(uint8_t secondHand);
    void setMode(Mode p_mode)
    {
        switch (p_mode)
        {
        case Mode::showTime:
            this->mode = showTime;
            break;
        case Mode::showPalette:
            this->mode = Mode::showPalette;
            break;

        default:
            break;
        }
    }
    void setHoursColor(CRGB p_color)
    {
        hoursLedsColor = p_color;
    }
    void update(NetworkManager *ntwmng);

    void setBrightness(unsigned int newB)
    {
        newB = newB < 0 ? 0 : newB;
        newB = newB > 255 ? 255 : newB;
        brightness = newB;
    }

    void readPotentiometer()
    {
        unsigned int val = analogRead(POTENTIOMETER_PIN);
        setBrightness(val);
    }

    void simulateFire();
    void changeColorWeather(NetworkManager *ntwmng);

private:
    Mode mode = Mode::showTime;

    CRGBPalette16 currentPalette;
    TBlendType currentBlending;

    int selectedLed = 0;

    const static unsigned int NUM_LEDS = 60;          // How many leds in your strip?
    unsigned int brightness = 3;                      // brightness for palettes
    const static unsigned int DATA_PIN = 25;          // ESP32 pin for LED control
    const static unsigned int POTENTIOMETER_PIN = 34; // ESP32 pin for Potentiometer -> brightness

    CRGB leds[NUM_LEDS];

    CRGB hoursLedsColor = CRGB::Red;
    CRGB minutesLedsColor = CRGB::Green;
    CRGB secondsLedsColor = CRGB::Blue;

    uint8_t startIndex = 0;
    uint8_t motionSpeed = -1;
};

#endif /* MYSTRIPLED_H */
