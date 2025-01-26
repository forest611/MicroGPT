#include <M5Unified.h>
#include "WiFiManager.h"
#include "AudioManager.h"

WiFiManager wifiManager;

void setup() {
  // M5StickCPlusの初期化
  auto cfg = M5.config();
  
  // HAT-SPK2の設定
  cfg.external_spk = true;
  cfg.external_speaker.hat_spk2 = true;
  cfg.output_power = true;
  
  M5.begin(cfg);
  
  // 画面の向きを設定（見やすい方向に）
  M5.Display.setRotation(3);
  
  // 画面の初期表示
  M5.Display.fillScreen(BLACK);
  M5.Display.setTextSize(2);
  M5.Display.setCursor(0, 0);
  M5.Display.println("MicroGPT");
  
  // WiFi接続
  if (!wifiManager.connect()) {
    M5.Display.println("Please restart");
    while(1) { delay(100); }
  }
}

void loop() {
  M5.update();  // ボタンの状態を更新
  
  // WiFi接続が切れた場合の再接続処理
  if (!wifiManager.isConnected()) {
    M5.Display.println("Reconnecting...");
    wifiManager.connect();
  }

  // ボタンAを押したら録音開始/停止
  if (M5.BtnA.wasPressed()) {
    int16_t* record_pointer = AudioManager::record(5, 8000);
    if (record_pointer == nullptr) {
      M5.Display.println("Recording Failed!");
    } else {
      M5.Display.println("Recording Finished!");
    }

    delay(1000);

    // 録音結果の再生
    if (!AudioManager::play(record_pointer, 5 * 8000, 8000)) {
      M5.Display.println("Playing Failed!");
    } else {
      M5.Display.println("Playing Finished!");
    }

    // メモリ解放
    heap_caps_free(record_pointer);

  }

  // 画面の初期化
  if (M5.BtnB.wasPressed()) {
    M5.Display.fillScreen(BLACK);
    M5.Display.setTextSize(2);
    M5.Display.setCursor(0, 0);
    M5.Display.println("MicroGPT");
  }
}
