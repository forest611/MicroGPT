# MicroGPT

M5StickCPlus用のChatGPTクライアント

## 機能

- WiFi接続
- 音声録音・再生
- ChatGPT連携（開発中）

## ハードウェア要件

- M5StickCPlus
- HAT-SPK2（スピーカーモジュール）

## 開発環境

- Arduino IDE
- ESP32ボードパッケージ
- M5Unifiedライブラリ

## セットアップ

1. Arduino IDEをインストール
2. ESP32ボードパッケージをインストール
3. M5Unifiedライブラリをインストール
4. `WiFiManager.h`のSSIDとパスワードを設定
5. `deploy.sh`を使用してビルド・デプロイ

## 使用方法

- ボタンA：音声録音（5秒）と再生
- ボタンB：画面クリア
