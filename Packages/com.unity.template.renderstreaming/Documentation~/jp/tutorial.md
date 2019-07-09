# 利用を開始するには

### パッケージのダウンロード

Render Streaming の利用を開始するためには、[Github Release](https://github.com/Unity-Technologies/UnityRenderStreaming/releases) からパッケージをダウンロードしてください。2つのパッケージをダウンロードします。

- `com.unity.webrtc.tgz`
- `com.unity.template.renderstreaming.tgz`

ダウンロードしたパッケージを所定のフォルダに移動することで、プロジェクトテンプレートとして利用することが可能です。

### プロジェクトテンプレート (Experimental)

プロジェクトテンプレートは、Unity で新規プロジェクトを作成するときにテンプレートとして選択することができます。

<img src="../images/template_in_unityhub.png" width=600 align=center>

テンプレートパッケージを利用する際は、2つの tgz ファイルをそれぞれ以下のフォルダに配置します。

`com.unity.template.renderstreaming.tgz` を以下のフォルダに移動

```
 <Unity Editor Root>/Data/Resources/PackageManager/ProjectTemplates
```

<img src="../images/install_renderstreaming_package.png" width=600 align=center>

`com.unity.webrtc.tgz` を以下のフォルダに移動

```&lt;Unity Editor Root&gt;/Data/Resources/PackageManager/Editor
<Unity Editor Root>/Data/Resources/PackageManager/Editor
```

<img src="../images/install_webrtc_package.png" width=600 align=center>

パッケージを移動後、Unity Hub を起動し新規プロジェクトを作成します。テンプレートに `Render Streaming` が追加されていることを確認してください。

### Web サーバの起動

Webサーバの実行ファイルは `Assets/~bin/` 以下に配置しています。ファイルを実行すると、コマンドプロンプトが起動します。

<img src="../images/launch_webserver_explorer.png" width=500 align=center>

コマンドプロンプトに以下のようなログが表示されていることを確認してください。 Web サーバのアドレスが表示されます。Web サーバのコマンドオプションの詳細は [Web サーバ](webserver.md)のページを参照してください。

<img src="../images/launch_webserver_cmd.png" width=400 align=center>

Webサーバのソースコードは [WebApp](WebApp) に配置しています。なお、ソースコードを実行するためには [Node.js](https://nodejs.org) をインストールする必要があります。公式サイトからインストーラをダウンロードしてインストールしてください。

### Unity 側の設定

サンプルシーン `Assets/Scenes/HDRPScene.unity` を開きます。

<img src="../images/hdrpscene.png" width=600 align=center>

ヒエラルキービューの `RenderStreaming` を選択してください。インスペクタビューの `Render Streaming` コンポーネントを確認してください。

<img src="../images/renderstreaming_inspector.png" width=300 align=center>

`URL Signaling` パラメータには、先ほど起動した Web サーバのアドレスを指定してください。インスペクタの各設定については Render Streaming クラスの[リファレンス](class-renderstreaming.md)を参照してください。

### Webページへのアクセス

[対応ブラウザ]()のリストに含まれているブラウザを起動します。
Web サーバのアドレスにアクセスしてください。以下のようにウェブページが表示されます。

<img src="../images/browser_hdrpscene.png" width=600 align=center>

### トラブルシューティング

- **Safari** 及び **iOS Safari** は、 **http** では WebRTC の機能を利用できません。 **https** を利用する必要があります。
- ブラウザの動作が確認できない場合、ブラウザのバージョンが古い可能性があります。最新のバージョンをインストールしてください。
- **Nvcodec** が利用可能なグラフィックボードが利用可能であることを確認してください。また、グラフィックドライバは最新のバージョンをインストールしてください。
- `com.unity.webrtc` のドキュメントも確認してください。

