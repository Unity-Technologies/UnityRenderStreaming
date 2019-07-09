# Render Streaming Class

**Render Streaming** は以下の機能を含みます。

- シグナリングサーバとの通信
- カメラからの入力をストリームとして送信
- ブラウザからの入力とイベントとの関連付け

<img src="/Users/kazuki/UnityRenderStreaming/Packages/com.unity.template.renderstreaming/Documentation~/images/renderstreaming_inspector.png" width=400 align=center>

## プロパティー

| パラメータ               | 説明                                                     | デフォルト                       |
| ------------------------ | -------------------------------------------------------- | -------------------------------- |
| URL Signaling            | シグナリングサーバのURLを指定します                      | http://localhost                 |
| URLs Ice Server          | STUN/TURN サーバをリストで指定します                     | \[stun:stun.l.google.com:19302\] |
| Interval                 | シグナリングサーバへのポーリング頻度を秒単位で指定します | 5                                |
| Capture Camera           | ストリーミング用のカメラを指定します                     |                                  |
| Array Button Click Event | ブラウザ上のボタンクリックイベントを登録します           |                                  |

## 詳細

