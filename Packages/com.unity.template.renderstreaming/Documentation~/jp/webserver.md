## Web アプリケーション

**Web アプリケーション** は2つの機能を提供しています。

- Unity と Web ブラウザ間のシグナリングの仲介
- Web ページの配信

## Web クライアント

**Render Streaming** の Web アプリケーションは WebRTC テクノロジを使用しているため、ブラウザが WebRTC に対応している必要があります。各種ブラウザの対応状況は[別ページ](https://caniuse.com/#search=webrtc)を確認してください。

動作を確認しているブラウザは以下になります。

- Chrome
- Safari
- Firefox
- iOS Safari
- Chrome for Android

> [!NOTE]
> **Safari** 及び **iOS Safari** は、 **http** では WebRTC の機能を利用できません。 **https** で利用する必要があります。

## Web サーバ

`Assets/bin~/` フォルダに実行ファイルを配置しています。コマンドライン上で起動してください。

<img src="../images/launch_webserver_explorer.png" width=500 align=center>

### コマンドオプション

|オプション|説明|デフォルト|
|-------|-----|-------|
|-h --help|ヘルプを表示します||
|-p —port \<number\>|ポート番号を指定します|80|
|-s --secure|https を使用します||
|-k —keyfile \<path\>|https で使用する秘密鍵ファイルを指定します|server.key|
|-c —certfile \<path\>|https で使用する証明書ファイルを指定します|server.cert|

### コマンドの例

以下のコマンドを実行すると http として起動します。ポートは80番を利用します。

```shell
.\webserver
```

以下のコマンドを実行すると https として起動します。ポートは443番を利用します。なお https として起動する際は証明書（`server.cert`）及び鍵（`server.key`）の指定が必要になります（[詳細](../en/https.md)）。

```shell
.\webserver -s -p 443
```

以下のコマンドを実行するとヘルプを表示します。

```shell
.\webserver -h
```
