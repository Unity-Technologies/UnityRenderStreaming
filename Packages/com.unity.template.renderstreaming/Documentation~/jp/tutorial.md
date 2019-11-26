# 利用を開始するには

### パッケージのダウンロード

Render Streaming の利用を開始する方法は 2 つあります。

- [Github レポジトリ](https://github.com/Unity-Technologies/UnityRenderStreaming)から最新のソースコードをダウンロード
- [Github Release](https://github.com/Unity-Technologies/UnityRenderStreaming/releases) から パッケージをダウンロード

パッケージをダウンロードする場合は、次の 2 つのパッケージをダウンロードしてください。

- `com.unity.renderstreaming-x.x.x-preview.tgz`
- `com.unity.template.renderstreaming-x.x.x-preview.tgz`

もう 1 つのパッケージは `com.unity.webrtc` リポジトリの [Github Release](https://github.com/Unity-Technologies/com.unity.webrtc/releases) からダウンロードします。

- `com.unity.webrtc-x.x.x-preview.tgz`

> [!NOTE]
> < これらの作業は暫定的なものです。将来的にはこれらのパッケージは PackageManager に登録される予定です。PackageManager から直接ダウンロード可能になれば、手動でのパッケージインストールは不要になります。 >

ダウンロードしたパッケージを所定のフォルダーに移動することで、プロジェクトテンプレートとして利用することが可能です。

### プロジェクトテンプレート (Experimental)

プロジェクトテンプレートとは、Unity で新規プロジェクトを作成するときに設定済みの Unity プロジェクトを選択できる機能です。

<img src="../images/template_in_unityhub.png" width=600 align=center>

テンプレートパッケージを利用する際は、次の 2 つの tgz ファイルをそれぞれ以下のフォルダーに配置します。

`com.unity.template.renderstreaming-x.x.x-preview.tgz` を以下のフォルダーに移動

```
  <Unity Editor Root>/Data/Resources/PackageManager/ProjectTemplates
```

<img src="../images/install_renderstreaming_package.png" width=600 align=center>

`com.unity.webrtc-x.x.x-preview.tgz` と`com.unity.renderstreaming-x.x.x-preview.tgz`を以下のフォルダーに移動

```&lt;Unity Editor Root&gt;/Data/Resources/PackageManager/Editor
<Unity Editor Root>/Data/Resources/PackageManager/Editor
```

<img src="../images/install_webrtc_package.png" width=600 align=center>

パッケージを移動後、Unity Hub を起動し新規プロジェクトを作成します。テンプレートに `Render Streaming` が追加されていることを確認してください。

### Web サーバーの起動

Web サーバーの実行ファイルは `Assets/~bin` 配下にあります。このファイルを実行するとコマンドプロンプトが表示されます。

<img src="../images/launch_webserver_explorer.png" width=500 align=center>

下記のログがコマンドプロンプトに表示されます。Web サーバーのアドレスもここに表示されています。Web サーバーコマンドのオプションは [Web サーバー](webserver.md)のページにまとめられています。

<img src="../images/launch_webserver_cmd.png" width=400 align=center>

Web サーバーのソースコードは [WebApp](WebApp) に配置しています。なお、ソースコードを実行するためには [Node.js](https://nodejs.org) をインストールする必要があります。公式サイトからインストーラーをダウンロードしてインストールしてください。

### Unity 側の設定

サンプルシーン `Assets/Scenes/samplescene.unity` を開きます。

<img src="../images/hdrpscene.png" width=600 align=center>

ヒエラルキービューの `RenderStreaming` を選択してください。インスペクタビューの `Render Streaming` コンポーネントを確認してください。

<img src="../images/renderstreaming_inspector.png" width=300 align=center>

`URL Signaling` パラメーターには、先ほど起動した Web サーバーのアドレスを指定してください。インスペクターの各設定については Render Streaming クラスの[リファレンス](class-renderstreaming.md)を参照してください。

### Web ページへのアクセス

[対応ブラウザー](index.md)のリストに含まれているブラウザーを起動します。
Web サーバーのアドレスにアクセスしてください。以下のように Web ページが表示されます。

<img src="../images/browser_hdrpscene.png" width=600 align=center>

上の画像のような結果にならなかった場合は、ブラウザーのバージョンが古い可能性があります。最新バージョンのブラウザーをインストールしてください。
