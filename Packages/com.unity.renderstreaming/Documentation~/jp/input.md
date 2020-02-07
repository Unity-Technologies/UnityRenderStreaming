# ブラウザー入力処理

ブラウザーからの入力は以下をサポートしています。

- マウスイベント
- キーボードイベント
- タッチイベント
- ボタンイベント

ブラウザーからバイト列を送信し、Unity 側で `New Input System` を利用しています。

### マウスイベント

|index|value|size|
|-------|-----|-----|
|0|0|1|
|1|position x|2|
|3|position y|2|
|5|button|1|

### キーボードイベント

|index|value|size|
|-------|-----|-----|
|0|1|1|
|1|key code|1|

### タッチイベント

|index|value|size|
|-------|-----|-----|
|0|2|1|
|1|phase|1|
|2|length|1|
|3|position x|2|
|5|position y|2|
|7|force|4|

### HTML ボタンイベント

HTML UI のイベントはボタンコンポーネントにのみ対応しています。
ボタンイベントのカスタマイズについては、 [Render Streaming 設定](class-renderstreaming.md#inspector-settings) を確認してください。

|index|value|size|
|-------|-----|-----|
|0|16|1|
|1|id|1|
