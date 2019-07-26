# The Web Application

The **Web application** 

- handles signaling between Unity and the Web browser
- is the location of the Web page 

## The Web Client

**Render Streaming** の Web アプリケーションは WebRTC テクノロジを使用しているため、ブラウザが WebRTC に対応している必要があります。各種ブラウザの対応状況は[別ページ](https://caniuse.com/#search=webrtc)を確認してください。

動作を確認しているブラウザは以下になります。

- Chrome
- Safari
- Firefox
- iOS Safari
- Chrome for Android

> [!NOTE]
> **Safari** 及び **iOS Safari** は、 **http** では WebRTC の機能を利用できません。 **https** で利用する必要があります。

## The Web Server

The Web server is distributed as a binary using [pkg](https://www.npmjs.com/package/pkg) in order to lessen the effort required to set up the environment. The .exe file is located in the `Assets/bin~/` folder.

<img src="../images/launch_webserver_explorer.png" width=500 align=center>

### Command Options

|Option|Details|Default|
|-------|-----|-------|
|-h --help|Show the help menu||
|-p —port \<number\>|Set the port number|80|
|-s --secure|Enable https||
|-k —keyfile \<path\>|Designate the private key file to use with https|server.key|
|-c —certfile \<path\>|Designate the certification file to use with https|server.cert|

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
