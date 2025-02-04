#ifndef AUDIO_MANAGER_H
#define AUDIO_MANAGER_H

#include <M5Unified.h>

class AudioManager {
private:
    static bool is_recording;
    static bool is_playing;

    static constexpr float TARGET_AMPLITUDE = 32767 * 0.8;

    static void logStatus(const char* message) {
        Serial.println(message);
    }

    static void normalize(int16_t* &data, int record_length) {
        // 最大振幅を検出
        int16_t max_amplitude = 0;
        for (int i = 0; i < record_length; i++) {
            int16_t abs_val = abs(data[i]);
            if (abs_val > max_amplitude) {
            max_amplitude = abs_val;
            }
        }

        // ノーマライズ係数を計算（最大振幅の80%を目標に）
        float normalize_factor = max_amplitude > 0 ? TARGET_AMPLITUDE / max_amplitude : 1.0;
        
        // 音量を調整
        for (int i = 0; i < record_length; i++) {
            float normalized = data[i] * normalize_factor;
            // クリッピング処理
            if (normalized > 32767) normalized = 32767;
            if (normalized < -32768) normalized = -32768;
            data[i] = (int16_t)normalized;
        }
    }

public:
    // 録音開始 使用後はheap_caps_freeで解放する
    static int16_t* record(double seconds, size_t sample_rate) {
        if (is_recording || is_playing) return nullptr;

        // メモリ確保
        int16_t* record_pointer = (int16_t*)heap_caps_malloc(seconds * sample_rate * sizeof(int16_t), MALLOC_CAP_8BIT);

        if (record_pointer == nullptr) {
            logStatus("Failed to allocate memory");
            return nullptr;
        }

        is_recording = true;
        logStatus("Recording...");

        // 録音開始処理
        if (!M5.Mic.isEnabled()){
            M5.Mic.begin();
            while(!M5.Mic.isEnabled()) { delay(10); }
        }

        // 録音開始
        size_t record_size = sample_rate * seconds;
        if (!M5.Mic.record(record_pointer, record_size,sample_rate)) {
            logStatus("Recording Failed!");            
        }

        is_recording = false;
        M5.Mic.end();

        return record_pointer;
    }

    static bool play(int16_t* record_pointer, size_t record_size, size_t sample_rate) {
        if (is_playing || is_recording) return false;

        is_playing = true;
        logStatus("Playing...");

        // 再生開始処理
        if (!M5.Speaker.isEnabled()){
            M5.Speaker.begin();
            while(!M5.Speaker.isEnabled()) { delay(10); }
        }

        normalize(record_pointer, record_size);

        // 再生開始
        if (!M5.Speaker.playRaw(record_pointer, record_size, sample_rate)) {
            logStatus("Playing Failed!");            
        }

        // 再生中待ち
        while (M5.Speaker.isPlaying()) {
            M5.update();
            delay(10);
        }

        is_playing = false;
        M5.Speaker.end();
        return true;
    }
};

// static変数の初期化
bool AudioManager::is_recording = false;
bool AudioManager::is_playing = false;

#endif // AUDIO_MANAGER_H
