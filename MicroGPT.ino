#include <M5Unified.h>
#include "WiFiManager.h"
#include "AudioManager.h"
#include "WebSocketManager.h"

WebsocketsClient WebSocketManager::client;

size_t sample_rate = 4000;
size_t record_seconds = 1;

void setup() {

  Serial.begin(115200);
  // M5StickCPlusの初期化
  auto cfg = M5.config();
  
  // HAT-SPK2の設定
  cfg.external_spk = true;
  cfg.external_speaker.hat_spk2 = true;
  cfg.output_power = true;
  
  M5.Speaker.setVolume(200);

  M5.begin(cfg);
  
  // 画面の向きを設定（見やすい方向に）
  M5.Display.setRotation(3);
  
  // 画面の初期表示
  M5.Display.fillScreen(BLACK);
  M5.Display.setTextSize(1);
  M5.Display.setCursor(0, 0);
  M5.Display.println("MicroGPT");
  
  // WiFi接続
  if (!WiFiManager::connect()) {
    M5.Display.println("Please restart");
    while(1) { delay(100); }
  }

  // WebSocket接続
  WebSocketManager::begin(onWebSocketMessage);

}

bool isRecording = false;

void loop() {
  M5.update();  // ボタンの状態を更新
  WebSocketManager::update();
  
  // WiFi接続が切れた場合の再接続処理
  if (!WiFiManager::isConnected()) {
    M5.Display.println("Reconnecting...");
    WiFiManager::connect();
  }

  // ボタンAを押している間は録音を行う
  if (M5.BtnA.isPressed()) {
    //押し始め
    if (!isRecording) {
      isRecording = true;
      WebSocketManager::send("start");
    }

    sendToWebSocket();
  }

  //押し終わり
  if (M5.BtnA.wasReleased()) {
    isRecording = false;
    WebSocketManager::send("end");
  }

  // ボタンBで再接続
  if (M5.BtnB.wasPressed()) {
    WebSocketManager::begin(onWebSocketMessage);
  }

  delay(10);

}

void sendToWebSocket(){

  // 録音開始
  int16_t* record_pointer = AudioManager::record(record_seconds, sample_rate);

  if (record_pointer == nullptr) {
    Serial.println("Recording Failed!");
  } 

  // // 録音データをCSVに変換
  // char* csv_buffer = (char*)heap_caps_malloc(record_seconds * sample_rate * sizeof(char), MALLOC_CAP_8BIT);
  // int16ArrayToCsv(record_pointer, record_seconds * sample_rate, csv_buffer, sizeof(csv_buffer));

  // 録音データをWebSocketに送信
  Serial.println("Sending record...");

  WebSocketManager::sendBinary((char*)record_pointer, record_seconds * sample_rate * sizeof(int16_t));

  // 録音データを解放
  heap_caps_free(record_pointer);
  // heap_caps_free(csv_buffer);

  Serial.println("Sent!");
}

void onWebSocketMessage(WebsocketsMessage message) {
  // WebSocketからのメッセージを処理
  Serial.println(message.data());
}

void int16ArrayToCsv(int16_t* buffer, size_t size, char* output, size_t maxLen) {
    size_t pos = 0;
    for (size_t i = 0; i < size; i++) {
        pos += snprintf(output + pos, maxLen - pos, "%d%s", buffer[i], (i < size - 1) ? "," : "");
        if (pos >= maxLen) break;  // バッファオーバーフローを防ぐ
    }
}