#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include "WiFiPicker.h"

WiFiPicker wifiPicker;

void setup() {
	Serial.begin(115200);
	
    wifiPicker.connect();

    Serial.print("Connected! Ip: ");
    Serial.println(WiFi.localIP());
}

void loop() {

}
