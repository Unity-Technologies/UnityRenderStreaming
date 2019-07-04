# プラグインのビルド方法

### libwebrtc の組み込み

プラグインは **libwebrtc** に依存しているため、ビルドするために libwebrtc をスタティックリンクする必要があります。Github Release ページに `webrtc-win.zip` を配置しています。
 <img src="../Packages/com.unity.webrtc/Documentation~/images/libwebrtc_github_release.png" width=600 align=center>

zip ファイルを展開後、Plugin フォルダ直下に配置します。

<img src="../Packages/com.unity.webrtc/Documentation~/images/deploy_libwebrtc.png" width=500 align=center>

### ビルド

version 1.0 現在、ビルドは **Visual Studio 2017** を利用しています。version 1.2 以降は **CMake** への移行を検討しています。

### デバッグ

プラグインのランタイムデバッグを行うためには、WebRTCPlugin プロジェクトのプロパティを変更する必要があります。

Output Directory を `Packages\com.unity.webrtc\Runtime\Plugins\x86_64` に指定します。

<img src="../Packages/com.unity.webrtc/Documentation~/images/outputdirectory_config_vs2017.png" width=600 align=center>

Command に Unity の実行ファイルパス、Command Arguments にプロジェクトパスを指定してください。

<img src="../Packages/com.unity.webrtc/Documentation~/images/command_config_vs2017.png" width=600 align=center>