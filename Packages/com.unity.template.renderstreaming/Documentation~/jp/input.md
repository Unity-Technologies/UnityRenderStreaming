## ブラウザ入力処理

ブラウザからの入力は以下をサポートしています。

- マウスイベント
- キーボードイベント
- タッチイベント
- ボタンイベント

ブラウザからバイト列を送信し、Unity 側で `New Input System` を利用しています。

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

## HTML UI

HTML UI のイベントはのボタンコンポーネントにのみ対応しています。

### Button

|index|value|size|
|-------|-----|-----|
|0|16|1|
|1|id|1|