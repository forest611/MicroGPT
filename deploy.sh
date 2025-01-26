#!/bin/bash

# エラーが発生したら即座に終了
set -e

echo "🔨 MicroGPTのビルドとデプロイを開始します..."

# コンパイル
echo "📝 コンパイルを実行中..."
arduino-cli compile --fqbn esp32:esp32:m5stack_stickc_plus ./MicroGPT.ino

# アップロード
echo "📤 M5StickCPlusへアップロード中..."
arduino-cli upload -p /dev/cu.usbserial-495A2F8412 --fqbn esp32:esp32:m5stack_stickc_plus ./MicroGPT.ino

echo "✨ デプロイが完了しました！"
