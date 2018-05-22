/*
	WiFiPicker.cpp - Library for saving and adding WiFi networks.
	Created by Tim van den Essen, 2018.
*/

#include "WiFiPicker.h"
#include <FS.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include <map>

static const String beginHtml = "<!DOCTYPE html><html lang=\"en\"><head><title>AP Configure</title><style></style></head><body><table><tbody><tr><td><label for=\"ssid\">SSID</label></td><td><input id=\"ssid\"/></td></tr><tr><td><label for=\"pass\" >Password</label></td><td><input id=\"pass\" type=\"password\"/></td></tr><tr><td><button onclick=\"location.href = '/add?ssid=' + escape(document.getElementById('ssid').value) + '&pass=' + escape(document.getElementById('pass').value);\">Add</button></td></tr></tbody></table><br/><table><tbody>";
static const String endHtml = "</tbody></table></body></html>";
static const String configFile = "/WiFiPicker";

void WiFiPicker::init(String ssid, String pass, bool hidden) {
	if (pass != "" && pass.length() < 8) {
		Serial.println("Warning. You have to use a password of 8 or more characters.");
		pass = "8characters";
	}

	WiFi.softAPdisconnect(true);
	WiFi.setAutoConnect(false);

	_ssid = ssid != "" ? ssid : "ESP" + String(ESP.getChipId());
	_pass = pass;
	_hidden = hidden;

	SPIFFS.begin();
}

WiFiPicker::WiFiPicker()
{
	init("", "", false);
}

WiFiPicker::WiFiPicker(String ssid) {
	init(ssid, "", false);
}

WiFiPicker::WiFiPicker(String ssid, String pass) {
	init(ssid, pass, false);
}

WiFiPicker::WiFiPicker(String ssid, String pass, bool hidden) {
	init(ssid, pass, hidden);
}


bool WiFiPicker::start() {
	if (_pass == "8characters") {
		Serial.println("Your password has been changed to: \"" + _pass + "\" because it is less than 8 characters long.");
	}

	bool willConnect = tryConnect();

	if (willConnect) {
		return true;
	}

	createAP();
	return false;
}

void WiFiPicker::readConfig() {
	_ssids = std::map<String, String>();

	File file = SPIFFS.open(configFile.c_str(), "r");
	if (!file) {
		Serial.println("No config file found.");
		return;
	}

	while (file.available() > 0) {
		String ssid = file.readStringUntil('\n');
		String pass = file.readStringUntil('\n');

		ssid.remove(ssid.length() - 1);
		pass.remove(pass.length() - 1);

		_ssids[ssid] = pass;
	}

	file.close();
}

void WiFiPicker::writeConfig() {
	File file = SPIFFS.open(configFile.c_str(), "w");
	for (auto const item : _ssids) {
		file.println(item.first);
		file.println(item.second);
	}
	file.close();
}

bool WiFiPicker::tryConnectToSsid(const char* ssid, const char* pass) {
	WiFi.begin(ssid, pass);

	while (true) {
		delay(500);

		switch (WiFi.status()) {
		case WL_CONNECTED: {
			Serial.println("Connected!");
			return true;
		}
		case WL_NO_SHIELD: {
			Serial.println("No wifi shield.");
			return false;
		}
		case WL_NO_SSID_AVAIL: {
			Serial.println("No SSID available.");
			return false;
		}
		case WL_CONNECT_FAILED: {
			Serial.println("Connect failed.");
			return false;
		}
		case WL_CONNECTION_LOST: {
			Serial.println("Connect lost.");
			return false;
		}
		case WL_DISCONNECTED: {
			continue;
		}
		default: {
			Serial.println("Default case.");
			continue;
		}
		}
	}
}

bool WiFiPicker::tryConnect() {
	readConfig();

	for (auto const item : _ssids) {
		String ssid = item.first;
		String pass = item.second;
		Serial.println("Trying \"" + ssid + "\" : \"" + pass + "\"");

		if (tryConnectToSsid(ssid.c_str(), pass.c_str())) {
			return true;
		}
	}

	return false;
}

void WiFiPicker::reset() {
	SPIFFS.remove(configFile);
	_ssids = std::map<String, String>();
}

bool WiFiPicker::redirectoToIp() {
	if (server->hostHeader() == WiFi.softAPIP().toString()) {
		return false;
	}

	server->sendHeader("Location", "http://" + WiFi.softAPIP().toString(), true);
	server->send(302, "text/plain", "");
	server->client().stop();
	return true;
}

void WiFiPicker::addSsid(String ssid) {
	addSsid(ssid, "");
}

void WiFiPicker::addSsid(String ssid, String pass) {
	_ssids[ssid] = pass;
	writeConfig();
}

void WiFiPicker::removeSsid(String ssid, String password) {
	for (std::map<String, String>::iterator it = _ssids.begin(); it != _ssids.end(); it++) {
		if (it->first == ssid && it->second == password) {
			_ssids.erase(it);
			writeConfig();
			return;
		}
	}
}

void WiFiPicker::handleRoot() {
	if (redirectoToIp()) {
		return;
	}

	String result = beginHtml;
	for (auto const item : _ssids) {
		result += "<tr><td><button onclick=\"location.href='/remove?ssid=' + escape('" + item.first + "') + '&pass=' + escape('" + item.second + "') \">&times;</button></td><td>" + item.first + "</td><td>-</td><td>" + item.second + "</td></tr>";
	}

	result += endHtml;

	server->send(200, "text/html", result);
}

void WiFiPicker::handleAdd() {
	server->send(200, "text/html", "The ESP will now reboot.");
	String ssid = server->arg("ssid");
	String pass = server->arg("pass");

	Serial.println("SSID: " + ssid + " | PASS: " + pass);

	addSsid(ssid, pass);

	delay(500);

	ESP.restart();
}

void WiFiPicker::handleRemove() {
	String ssid = server->arg("ssid");
	String pass = server->arg("pass");
	removeSsid(ssid, pass);
	handleRoot();
}

void WiFiPicker::createAP() {
	Serial.println("Creating AP.");
	server = std::unique_ptr<ESP8266WebServer>(new ESP8266WebServer(80));

	DNSServer dnsServer;

	WiFi.mode(WIFI_AP);

	if (_pass == "") {
		WiFi.softAP(_ssid.c_str());
	}
	else {
		WiFi.softAP(_ssid.c_str(), _pass.c_str(), 1, _hidden);
	}

	IPAddress myIP = WiFi.softAPIP();
	Serial.print("AP IP address: ");
	Serial.println(myIP);

	dnsServer.start(53, "*", myIP);

	server->on("/add", std::bind(&WiFiPicker::handleAdd, this));
	server->on("/remove", std::bind(&WiFiPicker::handleRemove, this));
	server->onNotFound(std::bind(&WiFiPicker::handleRoot, this));

	server->begin();
	Serial.println("HTTP server started");

	while (true) {
		dnsServer.processNextRequest();
		server->handleClient();
	}
}
