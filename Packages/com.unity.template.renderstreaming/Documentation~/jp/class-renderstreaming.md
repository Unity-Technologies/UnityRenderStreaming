# Render Streaming クラス

**Render Streaming** は以下の機能を含みます。

- シグナリングサーバとの通信
- カメラからの入力をストリームとして送信
- ブラウザからの入力とイベントとの関連付け

<img src="../images/renderstreaming_inspector.png" width=500 align=center>

## プロパティー

| パラメータ                   | 説明                                                         | デフォルト                      |
| ---------------------------- | ------------------------------------------------------------ | ------------------------------- |
| **URL Signaling**            | シグナリングサーバのURLを指定します                          | http://localhost                |
| **Ice Servers**              | STUN/TURN サーバをリストで指定します                         |                                 |
| *Urls*                       | STUN/TURN サーバのURL を指定します                           | [stun:stun.l.google.com:19302\] |
| *Username*                   | TURN サーバを利用するときに使用するユーザ名                  |                                 |
| *Credencial*                 | TURN サーバを利用するときにサーバーにログインするときに使用する資格情報 |                                 |
| *Credencial Type*            | TURN サーバを利用するときにログイン時に使用される資格情報の種類を指定します | Password                        |
| **Interval**                 | シグナリングサーバへのポーリング頻度を秒単位で指定します     | 5                               |
| **Streaming Size**           | ストリーミングに使用するフレームバッファのサイズです         | 1280, 720                       |
| **Capture Camera**           | ストリーミング用のカメラを指定します                         |                                 |
| **Array Button Click Event** | ブラウザ上のボタンクリックイベントを登録します               |                                 |
| *Element Id*                 | ブラウザから送られる ID を指定します                         |                                 |
| *Click*                      | ボタンクリックイベントを登録します                           |                                 |

