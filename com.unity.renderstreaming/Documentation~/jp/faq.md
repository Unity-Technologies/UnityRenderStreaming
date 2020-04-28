# よくある質問

## トラブルに関する質問

### ブラウザーにストリームが表示されない

ブラウザーのバージョンが古い可能性があります。まずは最新のバージョンであるか確認してください。あるいは、ファイアウォールの設定によっては動作しない場合があるので、ファイアウォールを一度オフにして試してみてください。 

ブラウザーのコンソールログや Unity のデバッグログ上にエラーが出ていないかを確認してください。

### iOS Safari で動作しない

Web サーバーの[起動時オプション](webapp.md)を確認してください。**Safari** 及び **iOS Safari** は、**http** では WebRTC の機能を利用できません。 **https** を利用する必要があります。
また **iOS Safari**においては、 自己署名証明書によるサーバーの場合、WebSocketによるシグナリングは動作いたしません。詳しくは、[こちら](signalingprotocol.md)を確認してください。

### 実行時に初期化で失敗する

`Graphics API` 設定を確認の上、`com.unity.webrtc` の[動作要件](https://docs.unity3d.com/Packages/com.unity.webrtc@latest/jp/overview.html)と比較し、対応済みか確認してください。

ハードウェアエンコーダーを利用する場合は、 **Nvcodec** が利用可能なグラフィックボードが利用可能であることを確認してください。また、グラフィックドライバーは最新のバージョンをインストールしてください。

### Web サーバーの起動に失敗する

ポートがすでにほかのサービスに使われていないか確認してください。

### uGUIがブラウザ経由で操作できない

実行されているアプリケーションにフォーカスが当たっている状態でなければ操作できません。
この問題は、 `com.unity.inputsystem` での対応状況に依存しています。

## 利用環境に関する質問

### 利用可能な Unity バージョンは？

Unity 2019.3 で利用可能です。

### 利用可能な OS は？

Windows10 x64, Linux x64, MacOS で利用可能です。
UWP(Universal Windows Platform), iOS や Android などのモバイル端末には未対応です。 

### ハードウェアの制約は？

ハードウェアエンコードについては、 **NVCodec** を利用しているために、 NVIDIA のグラフィックボードが必須になります。NVIDIA の[サイト](https://developer.nvidia.com/video-encode-decode-gpu-support-matrix)で、NVCodec が利用可能なグラフィックボードであることを確認してください。

### 利用可能なブラウザーは？

各種ブラウザーの WebRTC の対応状況に依存しています。

## 機能に関する質問

### 複数の端末に配信できる？

可能です。ただし、配信端末の数が増えるほど、配信側に負荷がかかります。最新の デスクトップ PC であれば 720p の解像度で 5 台程度の配信は可能ですが、それ以上は [SFU](https://webrtcglossary.com/sfu/) と呼ばれる配信サーバーを介して利用する必要があります。

### Simulcast には対応している？

現在未対応です。

### 複数のカメラの映像を配信することは可能？

可能です。なお、ハードウェアエンコーダーを利用する場合は、同時に利用可能なカメラの数が制限される場合があります。NVIDIA の場合はこの[ドキュメント](https://developer.nvidia.com/video-encode-decode-gpu-support-matrix)を参照してください。

### 画像品質を上げるには？

画像解像度を `CameraStreamer` のインスペクタで指定することは可能です。しかし、ネットワークの状況によって品質が変化することに注意してください。

### リアルタイムレイトレーシングには対応している？

利用可能です。詳細は HDRP の[ドキュメント](https://docs.unity3d.com/Packages/com.unity.render-pipelines.high-definition@7.1/manual/Ray-Tracing-Getting-Started.html)を参照してください。

## サポートに関する質問

### Unity から技術的なサポートは受けられますか？
Unity フォーラムに専用の[カテゴリ](https://forum.unity.com/forums/unity-render-streaming.413/)がありますので、質問することが可能です。
また、バグや機能要望については [Github Issues](https://github.com/Unity-Technologies/UnityRenderStreaming/issues) にご報告ください。

### Unity を GCP などにホスティングしてレンダリングサーバーとして利用できますか？

Unity はレンダリングサーバーとしての用途のサポートは行っていません。
