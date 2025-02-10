#ifndef WEBSOCKET_MANAGER_H
#define WEBSOCKET_MANAGER_H

#include <ArduinoWebsockets.h>

using namespace websockets;

class WebSocketManager {
private:
    WebsocketsClient client;     // staticを外す
    bool initialized;            // staticを外す
    bool connectionActive;       // staticを外す
    String hostUri;              // 接続先（URI）をメンバ変数で持つ

    // 接続処理を行う
    bool connect() {
        if (!initialized) {
            return false;
        }

        if (connectionActive) {
            // すでに接続済みならそのままtrueを返す
            return true;
        }

        // hostUriはコンストラクタで渡される
        bool success = client.connect(hostUri);
        if (success) {
            connectionActive = true;
            M5.Display.println("WebSocket Connected to: " + hostUri);
        } else {
            M5.Display.println("Connection failed: " + hostUri);
        }
        
        return success;
    }

public:
    // コンストラクタで接続先URLを受け取るようにする
    WebSocketManager(const String& uri) : initialized(false), connectionActive(false), hostUri(uri) {
    }

    // 初期化＋メッセージコールバックのセット
    void begin(std::function<void(WebsocketsMessage)> onMessage) {
        if (initialized) {
            return;
        }
        initialized = true;
        connectionActive = false;

        // コールバックを登録
        client.onMessage(onMessage);
        
        // 初回接続を試みる
        if (!connect()) {
            M5.Display.println("Initial connection failed. Will retry in update().");
        }
    }

    // 切断処理
    void disconnect() {
        if (connectionActive) {
            client.close();
            connectionActive = false;
            M5.Display.println("WebSocket Disconnected: " + hostUri);
        }
    }

    // テキストメッセージの送信
    bool send(const String& message) {
        if (!connectionActive && !connect()) {
            return false;
        }
        return client.send(message);
    }

    // バイナリデータの送信
    bool sendBinary(const char* message, size_t length) {
        if (!connectionActive && !connect()) {
            return false;
        }
        return client.sendBinary(message, length);
    }

    // 接続中かどうか
    bool isConnected() {
        if (!connectionActive) {
            return false;
        }
        bool available = client.available();
        if (!available) {
            // 切断されていたらフラグを変更
            connectionActive = false;
        }
        return available;
    }

    // 定期的に呼び出し、接続状態の更新や再接続などを行う
    void update() {
        if (connectionActive) {
            client.poll();
        } else if (initialized) {
            // 接続が切れている場合は再接続を試みる
            connect();
        }
    }
};
#endif // WEBSOCKET_MANAGER_H