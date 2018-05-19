esp8266-wifi-picker
-

This library makes it easy to dynamically add/remove saved WiFi access points.
Here's an example sketch:

```cpp
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
