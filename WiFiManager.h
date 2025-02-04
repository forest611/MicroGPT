#ifndef NETWORK_MANAGER_H
#define NETWORK_MANAGER_H

#include <M5Unified.h>
#include <WiFi.h>

class WiFiManager {
private:
    // WiFi設定
    static const char* ssid;      // ここにSSIDを設定
    static const char* password;   // ここにパスワードを設定
    
    static void displayStatus(const char* message) {
        M5.Lcd.println(message);
    }

public:

    static bool connect() {
        displayStatus("Connecting to WiFi...");
        
        WiFi.begin(ssid, password);
        
        // WiFi接続を試行（最大20秒）
        int attempts = 0;
        while (WiFi.status() != WL_CONNECTED && attempts < 20) {
            delay(1000);
            attempts++;
            M5.Lcd.print(".");
        }
        
        if (WiFi.status() == WL_CONNECTED) {
            displayStatus("WiFi Connected!");
            M5.Lcd.println(WiFi.localIP().toString());
            return true;
        } else {
            displayStatus("WiFi Connection Failed");
            return false;
        }
    }
    
    static bool isConnected() {
        return WiFi.status() == WL_CONNECTED;
    }
    
    static void disconnect() {
        WiFi.disconnect(true);
        WiFi.mode(WIFI_OFF);
        displayStatus("WiFi Disconnected");
    }
};

const char* WiFiManager::ssid = "Buffalo-G-AF20";
const char* WiFiManager::password = "37xk647bmgfgv";

#endif // NETWORK_MANAGER_H
