/*
	WiFiPicker.h - Library for saving and adding WiFi networks.
	Created by Tim van den Essen, 2018.
*/
#ifndef WiFiPicker_h
#define WiFiPicker_h

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <FS.h>
#include <map>

struct WiFiPicker {
public:
	WiFiPicker();
	WiFiPicker(String ssid);
	WiFiPicker(String ssid, String pass);
	WiFiPicker(String ssid, String pass, bool hidden);
	bool start();
	void reset();
	void addSsid(String ssid);
	void addSsid(String ssid, String password);
	void removeSsid(String ssid, String password);
private:
	void init(String ssid, String pass, bool hidden);
	void handleRoot();
	void handleAdd();
	void handleRemove();
	bool tryConnectToSsid(const char*, const char*);
	bool tryConnect();
	void createAP();
	bool redirectoToIp();
	String _ssid;
	String _pass;
	bool _hidden;
	std::map<String, String> _ssids;
	void readConfig();
	void writeConfig();
	std::unique_ptr<ESP8266WebServer> server;
};

#endif
