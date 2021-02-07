#include <Arduino.h>
#include <WiFiManager.h>
#include "time.h"
#include <FastLED.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include "global.h"
#include "myStripLed.h"

// How many leds in your strip?
//#define NUM_LEDS 60
//#define BRIGHTNESS 30
//#define DATA_PIN 25

const char *ssid = "SFR_0E381";
const char *password = "39vuqw5msd68qjti5mmb";

const char *ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 3600;
const int daylightOffset_sec = 3600;

AsyncWebServer server(80);
WiFiManager wm;

extern CRGBPalette16 myRedWhiteBluePalette;
extern const TProgmemPalette16 myRedWhiteBluePalette_p PROGMEM;

MyStripLed myStripLed;

// For led chips like WS2812, which have a data line, ground, and power, you just
// need to define DATA_PIN.  For led chipsets that are SPI based (four wires - data, clock,
// ground, and power), like the LPD8806 define both DATA_PIN and CLOCK_PIN
// Clock pin only needed for SPI based chipsets when not using hardware SPI

//const char* ssid     = "SFR_EBEF";
//const char* password = "c6kphxmvqdhsflvgwaj7";

void setWifi()
{
  //----------------------------------------------------WIFI STATTION
  WiFi.mode(WIFI_STA);

  if (!wm.autoConnect(ssid, password))
    Serial.println("Erreur de connexion.");
  else
    Serial.println("Connexion etablie!");

  Serial.println("\n");
  Serial.println("Connexion etablie!");
  Serial.print("Adresse IP: ");
  Serial.println(WiFi.localIP());

  // Init and get the time
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  //myStripLed.printLocalTime();

  //disconnect WiFi as it's no longer needed
  /* WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);
  */
}

void setSPIFFS()
{ //SPI File System
  //----------------------------------------------------SPIFFS
  if (!SPIFFS.begin())
  {
    Serial.println("Erreur SPIFFS...");
    return;
  }

  File root = SPIFFS.open("/");
  File file = root.openNextFile();

  while (file)
  {
    Serial.print("File: ");
    Serial.println(file.name());
    file.close();
    file = root.openNextFile();
  }
}

void setServer()
{
  //----------------------------------------------------SERVER
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/index.html", "text/html");
  });

  server.on("/script_led.js", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/script_led.js", "text/javascript");
  });

  server.on("/submitPaletteForm", HTTP_POST, [](AsyncWebServerRequest *request) {
    //showPalette=true;
    Serial.println("post");
    if (request->hasParam("selected_item", true))
    {
      String message;
      message = request->getParam("selected_item", true)->value();
      int secondHand = message.toInt();
      Serial.println(message);
      Serial.println(secondHand);

      myStripLed.setMode(1);
      myStripLed.changePalette(secondHand);
    }
    request->send(204);
  });

  server.on("/submitRGBForm", HTTP_POST, [](AsyncWebServerRequest *request) {
    //showPalette=false;
    Serial.println("post");
    if (request->hasParam("R", true) && request->hasParam("G", true) && request->hasParam("B", true))
    {
      String red = request->getParam("R", true)->value();
      String green = request->getParam("G", true)->value();
      String blue = request->getParam("B", true)->value();
      myStripLed.setHoursColor(CRGB(red.toInt(), green.toInt(), blue.toInt()));
    }
    request->send(204);
  });

  server.on("/submitHSVForm", HTTP_POST, [](AsyncWebServerRequest *request) {
    //showPalette=false;
    Serial.println("post");
    if (request->hasParam("H", true) && request->hasParam("S", true) && request->hasParam("V", true))
    {
      String hue = request->getParam("H", true)->value();
      String sat = request->getParam("S", true)->value();
      String val = request->getParam("V", true)->value();
      myStripLed.setHoursColor(CHSV(hue.toInt(), sat.toInt(), val.toInt()));
    }
    request->send(204);
  });

  server.on("/showTime", HTTP_POST, [](AsyncWebServerRequest *request) {
    myStripLed.setMode(0);
    request->send(204);
  });

  server.begin();
}

StaticJsonDocument<1000> requestJsonApi()
{
  HTTPClient client;
  String apiKey = "0b101241c6ff934b5f348500b3469c60";
  String query = "q=Lyon,fr&units=metric&appid=" + apiKey + "&lang=fr";
  String url = String("http://api.openweathermap.org/data/2.5/weather") + String('?') + query;
  Serial.println('\n' + url + '\n');

  client.begin(url);
  int httpCode = client.GET();
  StaticJsonDocument<1000> doc;
  // DynamicJsonDocument doc(2048); // use if size > 1kB

  if (httpCode > 0)
  {
    String payload = client.getString();
    Serial.println("http request api satuscode: " + String(httpCode));
    Serial.println(payload);

    char json[500];
    payload.replace(" ", "");
    payload.replace("\n", "");
    payload.trim();
    //// payload.remove(0, 1);
    payload.toCharArray(json, 1000);
    Serial.println(payload);

    if (deserializeJson(doc, json) == DeserializationError::Ok)
    {
      client.end();
      return doc;
    }
    else
    {
      client.end();
      return doc;
      Serial.println(String("deserialisation failed"));
    }
  }
  else
  {
    client.end();
    return doc;
    Serial.println("Error on HTTP request");
  }
}

void changeColorWeather()
{
  StaticJsonDocument<1000> doc = requestJsonApi();
  const char *name = doc["name"];
  Serial.println(name);

  int id = doc["id"];
  const char *description = doc["weather"][0]["description"];

  Serial.println(String(id));
  Serial.println(description);

  myStripLed.setMode(1);
  uint8_t newPalette = 45;
  const char *weather = doc["weather"][0]["main"];
  int weatherId = doc["weather"][0]["id"];
  Serial.println(weather);

  Serial.println(String(weatherId / 100));

  switch (int(weatherId / 100))
  {
  case 2:
  case 3:
  case 6:
  case 5:
  case 7:
    newPalette = 25;
    break;

  case 8:
    newPalette = 35;
    break;

  default:
    break;
  }

  myStripLed.changePalette(newPalette);
}

void setup()
{
  Serial.begin(115200);

  setWifi();
  setSPIFFS();
  setServer();

  delay(5000);

  changeColorWeather();
}

void loop()
{
  //delay(1000);
  myStripLed.update();
  //FastLED.show();
  FastLED.delay(1000 / 10);
}
