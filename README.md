WiFiPicker
-

This library makes it easy to dynamically add/remove saved WiFi access points.

When you call `WiFiPicker::connect()`, it'll retrieve the stored SSIDs and passwords and try them.  
If they all fail, an access point will be launched for you to fill in a new ssid.


Here's an example sketch:
```cpp
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <WiFiPicker.h>

WiFiPicker wifiPicker;

void setup() {
  Serial.begin(115200);
  
  wifiPicker.connect();
	
  Serial.print("Connected! Ip: ");
  Serial.println(WiFi.localIP());
}

void loop() {

}
```
Yes, it is that easy!

Also, here is a screenshot of the webpage.

![](https://ratelimited.me/gwsmjp.png)
