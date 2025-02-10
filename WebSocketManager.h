#include <ArduinoWebsockets.h>

using namespace websockets;

class WebSocketManager {
private:
    static WebsocketsClient client;
    static bool initialized;
    static bool connectionActive;

    static bool connect() {
        if (!initialized) {
            return false;
        }

        if (connectionActive) {
            return true;
        }

        bool success = client.connect("ws://192.168.11.33:3000/transcribe");
        if (success) {
            connectionActive = true;
            M5.Display.println("WebSocket Connected!");
        } else {
            M5.Display.println("Connection failed.");
        }
        
        return success;
    }

public:
    static void begin(std::function<void(WebsocketsMessage)> onMessage) {
        if (initialized) {
            return;
        }

        initialized = true;
        connectionActive = false;
        client.onMessage(onMessage);
        
        if (!connect()) {
            M5.Display.println("Initial connection failed. Will retry in loop.");
        }
    }

    static void disconnect() {
        if (connectionActive) {
            client.close();
            connectionActive = false;
        }
    }

    static bool send(String message) {
        if (!connectionActive && !connect()) {
            return false;
        }
        return client.send(message);
    }

    static bool sendBinary(char* message, size_t length) {
        if (!connectionActive && !connect()) {
            return false;
        }
        return client.sendBinary(message, length);
    }

    static bool isConnected() {
        if (!connectionActive) {
            return false;
        }
        bool available = client.available();
        if (!available) {
            connectionActive = false;
        }
        return available;
    }

    static void update() {
        if (connectionActive) {
            client.poll();
        } else if (initialized) {
            // 接続が切れている場合は再接続を試みる
            connect();
        }
    }
};