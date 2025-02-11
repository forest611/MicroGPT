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

    // ノーマライズ関数
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

    static bool enableMic() {
        M5.Speaker.end();
        M5.Mic.begin();
        
        if (M5.Mic.begin()) {
            logStatus("Mic Begin Success");
        }
        
        while (!M5.Mic.isEnabled()) { 
            M5.update();
            delay(10);    
        }
        logStatus("Mic Enabled");
        return true;
    }
    
    static bool enableSpeaker() {
        M5.Mic.end();
        
        if (M5.Speaker.begin()) {
            logStatus("Speaker Begin Success");
        }

        while (!M5.Speaker.isEnabled()) { 
            M5.update();
            delay(10);    
        }
        logStatus("Speaker Enabled");
        return true;
    }

    // 録音開始
    static bool record(int16_t* record_pointer, double seconds, size_t sample_rate) {

        if (!M5.Mic.isEnabled()) {
            logStatus("Mic is not enabled");
            return false;
        }

        if (is_recording || is_playing) return false;

        // メモリの確保ができていなかったら終了
        if (record_pointer == nullptr) {
            logStatus("Failed to allocate memory");
            return false;
        }

        is_recording = true;

        // 録音開始
        size_t record_size = sample_rate * seconds;
        
        if (!M5.Mic.record(record_pointer, record_size,sample_rate)) {
            logStatus("Recording Failed!");            
        }

        // 録音中待ち
        while (!M5.Mic.isRecording()) { 
            M5.update();
            delay(10);    
        }

        // ノーマライズ
        normalize(record_pointer, record_size);

        is_recording = false;

        return true;
    }
    
    static bool play(int16_t* record_pointer, size_t record_size, size_t sample_rate) {

        if (!M5.Speaker.isEnabled()) {
            logStatus("Speaker is not enabled");
            return false;
        }

        if (is_playing || is_recording) return false;

        is_playing = true;

        // normalize(record_pointer, record_size);

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

        return true;
    }
};

// static変数の初期化
bool AudioManager::is_recording = false;
bool AudioManager::is_playing = false;

#endif // AUDIO_MANAGER_H
