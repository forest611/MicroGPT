#include <M5Unified.h>
#include "WebSocketManager.h"

//WebSocketClientの初期化
WebsocketsClient WebSocketManager::client;
bool WebSocketManager::initialized = false;
bool WebSocketManager::connectionActive = false;

class WhisperAPI {

private:

    static const size_t sample_rate = 16000;
    static const size_t record_seconds = 1;

    static bool isRecording;
    // 録音データのポインタ
    static int16_t* record_pointer;

public:
    static bool startListening(){

        // マイク有効化
        AudioManager::enableMic();
    
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

    static bool finishListening(){
        // スピーカー有効化
        // AudioManager::enableSpeaker();

        isRecording = false;
        WebSocketManager::send("end");

        // 録音データを解放
        heap_caps_free(record_pointer);

        Serial.println("Listening Finished");

        return true;
    }

    static void transcribe(){

        if (!isRecording){
            startListening();
        }

        if (AudioManager::record(record_pointer,record_seconds, sample_rate)){
            WebSocketManager::sendBinary((char*)record_pointer, record_seconds * sample_rate * sizeof(int16_t));
            return;
        }
        Serial.println("Recording Failed");  
    }
};

// WhisperAPIの静的メンバ変数の定義
bool WhisperAPI::isRecording = false;
int16_t* WhisperAPI::record_pointer = nullptr;