# Render Streaming WebApp

### Web アプリケーションのビルド

Web サーバは環境構築の手間を省略するために、[pkg](https://www.npmjs.com/package/pkg) を利用してバイナリに変換しますja。以下コマンドでビルド可能です。

```
npm run pack
```

### Web アプリケーションの編集

Web アプリケーションのソースコードは  `WebApp` フォルダに配置しています。ソースコードを利用するには [Node.js](https://nodejs.org/) をインストールする必要があります。

**Node.js** を利用してコマンドを実行する場合は以下のようになります。

```shell
npm run start
```

以下のコマンドでヘルプを表示します。

```shell
npm run start -- -h
```

また、以下のコマンドで `ts-node` を利用してサーバを起動します。

```shell
npm run dev
```

### Web API の確認

Render Streaming のシグナリングはシンプルな Web API で実現しています。API の確認には [Postman](https://www.getpostman.com/) を利用することができます。

Postman で `WebApp/test/renderstreaming.postman_collection.json` をインポートすると、API の一覧を確認することができます。また、Web サーバを起動した状態であれば API の動作テストを行うことができます。

<img src="/Users/kazuki/UnityRenderStreaming/Packages/com.unity.template.renderstreaming/Documentation~/images/postman_example.png" width=600 align=center>