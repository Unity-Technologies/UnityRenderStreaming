# Browser Input Process

Unity Render Streaming は、ブラウザからのユーザー入力をサポートしています。ブラウザ上での操作をあたかも Unity 上で操作しているかのように再現できます。

## Input devices

The following browser inputs are supported. 

- Mouse events
- Keyboard events
- Touch events
- Button events

The browser sends byte sequences, and Unity uses the `New Input System`.

## Data Format

### Mouse Events

|index|value|size|
|-------|-----|-----|
|0|0|1|
|1|position x|2|
|3|position y|2|
|5|button|1|

### Keyboard Events

|index|value|size|
|-------|-----|-----|
|0|1|1|
|1|key code|1|

### Touch Events

タッチイベントは検出された指の本数に応じてデータサイズが可変になります。 1バイト目の `length` の値が検出された指の本数になります。

|index|value|size|
|-------|-----|-----|
|0|2|1|
|1|phase|1|
|2|length|1|
|3|position x|2|
|5|position y|2|
|7|force|4|

### ゲームパッドイベント

ゲームパッドのイベントは4種類あり、種類によってデータフォーマットが異なります。

| event name | value |
|-------|-----|
| button down | 0 |
| button up | 1 |
| button pressed | 2 |
| axis | 3 |

`button down` 、 `button up` 、 `button pressed` の場合は以下。

|index|value|size|
|-------|-----|-----|
|0|5|1|
|1|event type|1|
|2|button index|1|
|3|value|8|

`axis` の場合は以下。

|0|5|1|
|1|event type|1|
|2|button index|1|
|3|axis x|8|
|11|axis y|8|

## マルチユーザー

複数ユーザーの入力を制御するために `RemoteInputReceiver` クラスが利用できます。  `RemoteInputReceiver.Create` を呼び出して、 `RemoteInput` のインスタンスを作成します。次に `RTCDataChannel` から受け取ったメッセージを `RemoteInput.ProcessInput` メソッドに受け渡します。

```CSharp
// RemoteInput のインスタンスを作成
RemoteInput input = RemoteInputReceiver.Create();
channel.OnMessage = bytes => input.ProcessInput(bytes);
```

`RemoteInput` のインスタンスから入力デバイスを取得して、デバイスの値を参照することで入力を制御することができます。例えばキーボード入力は以下のように記述します。

```CSharp
// キーボードのデバイスを取得して、w キー押下時に処理
Keyboard keyboard = input.RemoteKeyboard;
if(keyboard.wKey.isPressed)
{
   // ... 
}
```