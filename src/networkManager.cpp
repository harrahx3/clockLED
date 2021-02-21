#include <Arduino.h>
#include <WiFiManager.h>
#include "time.h"
#include <FastLED.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include "networkManager.h"

NetworkManager::NetworkManager()
{
    const uint16_t port = 80;
    server = new AsyncWebServer(port);
    myStripLed = new MyStripLed();
}

NetworkManager::NetworkManager(MyStripLed *p_myStripLed)
{
    const uint16_t port = 80;
    server = new AsyncWebServer(port);
    myStripLed = p_myStripLed;
}

void NetworkManager::setWifi()
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

void NetworkManager::setSPIFFS()
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

void NetworkManager::setServer()
{
    myStripLed->setMode(MyStripLed::Mode::showPalette);

    //----------------------------------------------------SERVER
    server->on("/", HTTP_GET, [this](AsyncWebServerRequest *request) {
        request->send(SPIFFS, "/index.html", "text/html");
    });

    server->on("/script_led.js", HTTP_GET, [this](AsyncWebServerRequest *request) {
        request->send(SPIFFS, "/script_led.js", "text/javascript");
    });

    server->on("/submitPaletteForm", HTTP_POST, [this](AsyncWebServerRequest *request) {
        //showPalette=true;
        Serial.println("post submitPaletteForm");
        if (request->hasParam("selected_item", true))
        {
            String message;
            message = request->getParam("selected_item", true)->value();
            int secondHand = message.toInt();
            Serial.println(message);
            Serial.println(secondHand);

            myStripLed->setMode(MyStripLed::Mode::showPalette);
            myStripLed->changePalette(secondHand);
        }
        request->send(204);
    });

    server->on("/submitRGBForm", HTTP_POST, [this](AsyncWebServerRequest *request) {
        //showPalette=false;
        Serial.println("post submitRGBForm");
        if (request->hasParam("R", true) && request->hasParam("G", true) && request->hasParam("B", true))
        {
            String red = request->getParam("R", true)->value();
            String green = request->getParam("G", true)->value();
            String blue = request->getParam("B", true)->value();
            myStripLed->setHoursColor(CRGB(red.toInt(), green.toInt(), blue.toInt()));
        }
        request->send(204);
    });

    server->on("/submitHSVForm", HTTP_POST, [this](AsyncWebServerRequest *request) {
        //showPalette=false;
        Serial.println("post submitHSVForm");
        if (request->hasParam("H", true) && request->hasParam("S", true) && request->hasParam("V", true))
        {
            String hue = request->getParam("H", true)->value();
            String sat = request->getParam("S", true)->value();
            String val = request->getParam("V", true)->value();
            myStripLed->setHoursColor(CHSV(hue.toInt(), sat.toInt(), val.toInt()));
        }
        request->send(204);
    });

    server->on("/submitColorPicker", HTTP_POST, [this](AsyncWebServerRequest *request) {
        //showPalette=false;
        Serial.println("post submitColorPicker");
        if (request->hasParam("colorInput", true))
        {
            String colorCode = request->getParam("colorInput", true)->value();
            Serial.println("colorCode:");
            Serial.println(colorCode);
            Serial.println(colorCode.toInt());
            myStripLed->setHoursColor(CRGB(colorCode.toInt()));
        }
        request->send(204);
    });

    server->on("/showTime", HTTP_POST, [this](AsyncWebServerRequest *request) {
        Serial.println("post showTime");
        myStripLed->setMode(MyStripLed::Mode::showTime);
        request->send(204);
    });

    server->on("/onFire", HTTP_POST, [this](AsyncWebServerRequest *request) {
        Serial.println("post onFire");
        myStripLed->setMode(MyStripLed::Mode::onFire);
        request->send(204);
    });

    server->on("/showWeather", HTTP_POST, [this](AsyncWebServerRequest *request) {
        Serial.println("post showWeather");
        myStripLed->setMode(MyStripLed::Mode::showWeather);
        request->send(204);
    });

    server->on("/off", HTTP_POST, [this](AsyncWebServerRequest *request) {
        Serial.println("post off");
        myStripLed->setMode(MyStripLed::Mode::off);
        request->send(204);
    });

    server->begin();
}

StaticJsonDocument<1000> NetworkManager::requestJsonApi()
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
