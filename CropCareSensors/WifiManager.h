#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <WiFi.h>
#include <HTTPClient.h>

class WiFiManager {
private:
    const char* ssid;
    const char* password;
    
public:
    WiFiManager(const char* ssid, const char* password) 
        : ssid(ssid), password(password) {}
        
    void connect() {
        WiFi.begin(ssid, password);
        while (WiFi.status() != WL_CONNECTED) {
            delay(500);
            Serial.print(".");
        }
        Serial.println("WiFi connected");
    }
    
    bool isConnected() {
        return WiFi.status() == WL_CONNECTED;
    }
    
    void sendHTTPPost(const char* serverURL, const String& body) {
        if (!isConnected()) {
            Serial.println("WiFi not connected");
            return;
        }
        
        HTTPClient http;
        http.begin(serverURL);
        http.addHeader("Content-Type", "application/x-www-form-urlencoded");
        
        // Serial.println("Sending POST: " + body);
        int code = http.POST(body);
        String response = http.getString();
        
        // Serial.printf("POST returned %d\n", code);
        // Serial.println("Server response: " + response);
        http.end();
    }
};
#endif