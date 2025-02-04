#include <ArduinoWebsockets.h>

using namespace websockets;

class WebSocketManager {
private:
    static WebsocketsClient client;

    static void connect() {
        bool success = client.connect("ws://192.168.11.33:3000");

        if (!success) {
            M5.Display.println("Connection failed.");
        }

        M5.Display.println("WebSocket Connected!");
        
    }

public:

    static void begin(std::function<void(WebsocketsMessage)> onMessage) {

        if (isConnected()) {
            return;
        }

        client.onMessage(onMessage);
        connect();
    }

    static void disconnect() {
        client.close();
    }

    static bool send(String message) {
        return client.send(message);
    }

    static bool sendBinary(char* message, size_t length) {
        return client.sendBinary(message, length);
    }

    static bool isConnected() {
        return client.available();
    }

    static void update() {
        client.poll();
    }

};