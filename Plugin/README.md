# プラグイン

`com.unity.webrtc` が依存するネイティブプラグインのビルド及び配置の方法について説明します。

### libwebrtc の組み込み

プラグインは **libwebrtc** に依存しているため、ビルドするために libwebrtc をスタティックリンクする必要があります。Github Release ページに `webrtc-win.zip` を配置しています。
 <img src="../Packages/com.unity.webrtc/Documentation~/images/libwebrtc_github_release.png" width=600 align=center>

zip ファイルを展開後、Plugin フォルダ直下に配置します。

<img src="../Packages/com.unity.webrtc/Documentation~/images/deploy_libwebrtc.png" width=500 align=center>

### 開発環境

version 1.0 現在、ビルドは **Visual Studio 2017** を利用しています。

### プロジェクトの設定

プラグインの開発を行うためには、個別の環境に合わせて`WebRTCPlugin` プロジェクトのプロパティを変更する必要があります。

`Command` に Unity の実行ファイルパス、`Command Arguments` にプロジェクトパスを指定してください。この設定を行うことで、デバッグ実行時に Unity エディタが起動し、ブレークポイントが有効になります。

<img src="../Packages/com.unity.webrtc/Documentation~/images/command_config_vs2017.png" width=600 align=center>

### プラグインの配置

ビルド実行すると、`webrtc.dll` が `Packages\com.unity.webrtc\Runtime\Plugins\x86_64` に配置されます。このとき Unity のインスペクタ上で以下の設定になっていることを確認してください。

<img src="../Packages/com.unity.webrtc/Documentation~/images/inspector_webrtc_plugin.png" width=600 align=center>

