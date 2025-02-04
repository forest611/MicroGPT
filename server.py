import asyncio
import websockets

async def handler(websocket):
    """
    WebSocketクライアントが接続してきた際に呼び出されるハンドラ。
    (websocket.path は環境によっては存在しない場合があるため使わない)
    """
    print("Client connected.")

    try:
        # クライアントから届く各フレームを順次受信
        async for message in websocket:
            # message がバイナリ(bytes)かテキスト(str)かを判定
            if isinstance(message, bytes):
                print(f"Received binary data of length {len(message)}")
                # 受信データをファイルに追記 (モード: 'ab')
                with open("audio_data.pcm", "ab") as f:
                    f.write(message)
            else:
                # テキストフレームの場合
                print(f"Received text data: {message}")

    except websockets.exceptions.ConnectionClosed:
        print("Client disconnected.")

async def main():
    print("Starting server on port 8765...")
    # "ws://<サーバーIP>:8765" へアクセスすると handler() が呼ばれる
    async with websockets.serve(handler, "0.0.0.0", 3000):
        # 終了しないようにずっと待機
        await asyncio.Future()

if __name__ == "__main__":
    asyncio.run(main())
