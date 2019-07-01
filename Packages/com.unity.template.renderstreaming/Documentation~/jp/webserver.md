## Web サーバ

Web サーバのソースコードは [WebApp](WebApp) フォルダに配置しています。

### コマンドオプション

npm run start -- -s -p 443 

|オプション|説明|
|-------|-----|
|-h --help|ヘルプを表示します|
|-p --port|ポート番号を指定します|

### Web API

Unity Render Streaming のシグナリングは、HTTP プロトコルで実現しています。シグナリングとは、P2Pの通信経路確立のためにピア同士の情報をやりとりする仕組みです。
Web API の確認は [Postman](https://www.getpostman.com/) を利用します。
[ファイル](WebApp/test/renderstreaming.postman_collection.json) を Postman でインポートすることで確認できます。

<img src="../images/postman_example.png" width=600 align=center>