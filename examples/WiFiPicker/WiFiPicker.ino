#include "WiFiPicker.h"

WiFiPicker wifiPicker;

void setup() {
    Serial.begin(115200);

    wifiPicker.start();

    Serial.print("Connected! Ip: ");
    Serial.println(WiFi.localIP());
}

void loop() {

}
