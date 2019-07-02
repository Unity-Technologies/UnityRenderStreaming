## Web サーバ

Web サーバのソースコードは [WebApp](WebApp) フォルダに配置しています。

### コマンドオプション

|オプション|説明|デフォルト|
|-------|-----|-------|
|-h --help|ヘルプを表示します||
|-p —port \<number\>|ポート番号を指定します|80|
|-s --secure|https を使用します||
|-k —keyfile \<path\>|https で使用する秘密鍵ファイルを指定します|server.key|
|-c —certfile \<path\>|https で使用する証明書ファイルを指定します|server.cert|

### 例

以下のコマンドを実行すると http として起動します。ポートは80番を利用します。

```shell
npm run start
```

以下のコマンドを実行すると https として起動します。ポートは443番を利用します。

```shell
npm run start -- -s -p 443
```

以下のコマンドを実行するとヘルプを表示します。

```shell
npm run start -- -h
```

`ts-node` を利用してサーバを起動する場合は `dev` を使用します。

```shell
npm run dev
```

### Web API の確認

Unity Render Streaming のシグナリングは、HTTP プロトコルで実現しています。シグナリングとは、P2Pの通信経路確立のためにピア同士の情報をやりとりする仕組みです。
Web API の確認には [Postman](https://www.getpostman.com/) を利用することができます。Postman を開き、 `WebApp/test/renderstreaming.postman_collection.json` をインポートしてください。

<img src="../images/postman_example.png" width=600 align=center>