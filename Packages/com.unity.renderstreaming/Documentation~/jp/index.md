# com.unity.renderstreaming

- [English](../index.md)

このパッケージには Unity の [**Unity Render Streaming**](../../../com.unity.template.renderstreaming/Documentation~/index.md) を基に作られた公開 API とサンプルプロジェクトが含まれます。

現在、1 つのサンプルがあります。

- HDRP. 
  > このサンプルをインポートすると、HDRP *(com.unity.render-pipelines.high-definition)* パッケージと Unity が提供している HDRP サンプルアセットのインストールと設定が自動的に行われます。

## Windows 向けの簡単なチュートリアル

1. 既存のプロジェクトを開きます。または、新しいプロジェクトを作成します。
2. `Window` メニューをクリックし、`Package Manager` を開きます。次に、`Advanced` をクリックして `Show preview packages` にチェックを入れます。

<img src="../images/show_preview_packages.png" width=480 align=center>

3. 検索のモードを `All packages` にして、テキストボックスに `RenderStreaming` と入力します。

<img src="../images/render_streaming_package.png" width=480 align=center>

4. 右下の `Install` をクリックします。

5. 入力システムに関するダイアログボックスが表示された場合は、`Yes` をクリックします。

<img src="../images/input_system_backend.png" width=360 align=center>

6. `Import in project` をクリックして HDRP サンプルをインポートします。
この操作によって、HDRP パッケージ *(com.unity.render-pipelines.high-definition)* が自動的にインストールされます。

<img src="../images/hdrp_sample.png" width=480 align=center>

7. 下図の `Import Unity Package` ダイアログボックスが表示されたら、`Import` をクリックしてすべてのアセットをインポートします。

<img src="../images/hdrp_unitypackage.png" width=240 align=center>

8. すべてのインポートプロセスが終わった後、Project ビューから `Assets/SimpleScene` を開きます。

<img src="../images/simplescene.png" width=240 align=center>

9. `Edit/Render Streaming/Download web app` メニューアイテムをクリックして、あらかじめ用意されているウェブサーバーをダウンロードします。

<img src="../images/download_webapp.png" width=300 align=center>

10. ダウンロード先フォルダーを選択するウィンドウが表示されたら、`Select Folder` をクリックしてデフォルトのフォルダーにファイルをダウンロードします。

<img src="../images/select_download_folder.png" width=240 align=center>


11. ダウンロードが終了して、エクスプローラーの新しいウィンドウが開いたら、`webserver.exe` をクリックして Unity RenderStreaming ウェブサーバーを起動します。

<img src="../images/webserver.png" width=240 align=center>

12. Unity で再生モードに入ります。

<img src="../images/play_mode.png" width=240 align=center>

13. ブラウザーを立ち上げ、URL バーに `localhost` と入力し、中央に表示されている再生ボタンをクリックします。
クリックすると、Unity のビューがブラウザーに表示されます。

<img src="../images/browser.png" width=240 align=center>









