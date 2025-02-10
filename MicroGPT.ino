#include <M5Unified.h>
#include "WiFiManager.h"
#include "AudioManager.h"
#include "WebSocketManager.h"

WebsocketsClient WebSocketManager::client;
bool WebSocketManager::initialized = false;
bool WebSocketManager::connectionActive = false;

const size_t sample_rate = 16000;
const size_t record_seconds = 1;

const String ssid = "Buffalo-G-AF20";
const String password = "37xk647bmgfgv";

bool isRecording = false;

// 録音データのポインタ
int16_t* record_pointer = nullptr;

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
  if (!WiFiManager::connect(ssid, password)) {
    M5.Display.println("Please restart");
    while(1) { delay(100); }
  }

  // WebSocket接続
  WebSocketManager::begin(onWebSocketMessage);

  //スピーカー有効化
  AudioManager::enableSpeaker();
}

void loop() {

  M5.update();  // ボタンの状態を更新
  WebSocketManager::update();
  
  // WiFi接続が切れた場合の再接続処理
  if (!WiFiManager::isConnected()) {
    M5.Display.println("WiFi Disconnected Please restart");
    delay(1000);
  }

  // ボタンAを押している間は録音を行う
  if (M5.BtnA.isPressed()) {
    //押し始め
    if (!isRecording){
      startListening();
    }
    //録音中
    transcribe();
  }

  //押し終わり
  if (M5.BtnA.wasReleased()) {
    finishListening();    
    M5.Display.println("Transcribing...");
  }

}

bool startListening(){
  
  record_pointer = (int16_t*)heap_caps_malloc(record_seconds * sample_rate * sizeof(int16_t), MALLOC_CAP_DMA);
  if (record_pointer == nullptr) {
    Serial.println("Failed to allocate memory");
    return false;
  }

  isRecording = true;
  WebSocketManager::send("start");
  M5.Display.println("Listening...");
  Serial.println("Listening...");

  return true;
}

bool finishListening(){

  // スピーカー有効化
  // AudioManager::enableSpeaker();

  isRecording = false;
  WebSocketManager::send("end");

  // 録音データを解放
  heap_caps_free(record_pointer);

  Serial.println("Listening Finished");

  return true;
}

void transcribe(){

  if (AudioManager::record(record_pointer,record_seconds, sample_rate)){
    WebSocketManager::sendBinary((char*)record_pointer, record_seconds * sample_rate * sizeof(int16_t));
    return;
  }
  Serial.println("Recording Failed");  
}

void onWebSocketMessage(WebsocketsMessage message) {
  // WebSocketからのメッセージを処理
  Serial.println(message.data());
}
