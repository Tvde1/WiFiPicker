#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include "WiFiPicker.h"

WiFiPicker wifiPicker;

void setup() {
    wifiPicker.connect();

    Serial.print("Connected! Ip: ");
    Serial.println(WiFi.localIP());
}

void loop() {

}
