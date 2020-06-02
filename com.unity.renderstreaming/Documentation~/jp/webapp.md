# Web アプリケーション

**Web アプリケーション** は 2 つの機能を提供しています。

- Unity と Web ブラウザー間のシグナリングの仲介
- Web ページの配信

## 対応ブラウザ

**Unity Render Streaming** の Web アプリケーションは、 WebRTC テクノロジを使用しているため、ブラウザーが WebRTC に対応している必要があります。各種ブラウザーの対応状況は[別ページ](https://caniuse.com/#search=webrtc)を確認してください。

動作を確認しているブラウザーは以下になります。

| Browser                           | Windows            | Mac                | iOS                | Android            |
| --------------------------------- | ------------------ | ------------------ | ------------------ | ------------------ |
| Google Chrome                     | :white_check_mark: | :white_check_mark: |                    | :white_check_mark: |
| Safari                            |                    | :white_check_mark: | :white_check_mark: |                    |
| Firefox                           | :white_check_mark: |                    |                    |                    |
| Microsoft edge (Chromium based)   | :white_check_mark: |                    |                    |                    |

> [!NOTE]
> **Safari** 及び **iOS Safari** は、 **http** では WebRTC の機能を利用できません。 **https** で利用する必要があります。

## Web アプリケーション

Web アプリケーションのサンプルは **node.js** で開発されており、Unity プロジェクトとは別に管理されています。
Web アプリケーションの利用を開始するには、アプリケーションをダウンロードする必要があります。 

### ダウンロード

Unity Editor のメニューから、 **Edit / Render Streaming / Download web app** をクリックしてダウンロードします。


### 実行

Web アプリケーションのダウンロードが終了したら、コマンドライン上で起動してください。

![Launch webserver explorer](../images/launch_webserver_explorer.png)

### コマンドオプション

| オプション             | 説明                                       | デフォルト   |
| --------------------- | ----------------------------------------  | ----------- |
| -h --help             | ヘルプを表示します                          |             |
| -p --port \<number\>  | ポート番号を指定します                       | 80          |
| -s --secure           | `https` を使用します                      |            |
| -k --keyfile \<path\> | `https` で使用する秘密鍵ファイルを指定します | server.key  |
| -c --certfile \<path\>| `https` で使用する証明書ファイルを指定します | server.cert |
| -w --websocket        | シグナリングプロトコルとしてwebsocketを使用します。 |         |

### コマンドの例

以下のコマンドを実行すると `http` として起動します。ポートは **80** を利用します。

```shell
.\webserver
```

以下のコマンドを実行すると `https` として起動します。ポートは **443** を利用します。なお `https` として起動する際は証明書（`server.cert`）及び鍵（`server.key`）の指定が必要になります（[詳細](https.md)）。

```shell
.\webserver -s -p 443
```

以下のコマンドを実行するとシグナリングプロトコルとしてWebSocketを使用するモードで起動します。

```shell
.\webserver -w
```

以下のコマンドを実行するとヘルプを表示します。

```shell
.\webserver -h
```

### Web アプリケーションのカスタマイズ

