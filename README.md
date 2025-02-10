# MicroGPT

M5StickC Plusを使用して音声入力でChatGPTと会話できるデバイスを作成するプロジェクトです。

## 機能

- 音声入力による会話
- WebSocketを使用したサーバーとの通信
- M5StickC Plusのディスプレイによる状態表示
- HAT-SPK2を使用した音声出力

## 必要なハードウェア

- M5StickC Plus
- HAT-SPK2（スピーカーモジュール）

## セットアップ方法

1. Arduino IDEに必要なライブラリをインストール
   - M5Unified
   - WebSockets

2. M5StickC PlusにMicroGPT.inoを書き込み

## 使用方法

1. デバイスの電源を入れると自動的にWiFiに接続します
2. サーバーとの接続が確立すると会話が可能になります
3. ボタンを押して音声入力を開始し、会話を楽しんでください

## ファイル構成

- `MicroGPT.ino`: メインのArduinoスケッチ
- `WiFiManager.h`: WiFi接続管理
- `AudioManager.h`: 音声入出力管理
- `WebSocketManager.h`: WebSocket通信管理
- `monitor.sh`: シリアルモニター用スクリプト
- `deploy.sh`: デプロイ用スクリプト
