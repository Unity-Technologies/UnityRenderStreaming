## Web Applications

A **Web application** can be

- Unity - Web ブラウザ間のシグナリングの仲介
- Web ページの配置 



## Web クライアント



## Web サーバ

Web サーバは環境構築の手間を省略するために、[pkg](https://www.npmjs.com/package/pkg) を利用してバイナリとして配布しています。`Assets/bin~/` フォルダに実行ファイルを配置しています。

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

以下のコマンドを実行すると https として起動します。ポートは443番を利用します。

```shell
.\webserver -s -p 443
```

以下のコマンドを実行するとヘルプを表示します。

```shell
.\webserver -h
```

### Web アプリケーションの編集

Web アプリケーションのソースコードは [WebApp](WebApp) フォルダに配置しています。ソースコードを利用するには [Node.js](https://nodejs.org/) をインストールする必要があります。

**Node.js** を利用してコマンドを実行する場合は以下のようになります。

```shell
npm run start
```

以下のコマンドでヘルプを表示します。

```shell
npm run start -- -h
```

以下のコマンドで `ts-node` を利用してサーバを起動します。

```shell
npm run dev
```

### Web API の確認

Render Streaming のシグナリングはシンプルな Web API で実現しています。API の確認には [Postman](https://www.getpostman.com/) を利用することができます。

Postman で `WebApp/test/renderstreaming.postman_collection.json` をインポートすると、API の一覧を確認することができます。また、Web サーバを起動した状態であれば API の動作テストを行うことができます。

<img src="../images/postman_example.png" width=600 align=center>
