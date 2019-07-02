# Unity Render Streaming

<img src="https://img.shields.io/badge/unity-2019.1-green.svg?style=flat-square" alt="unity 2019.1">

**Unity Render Streaming** は、ブラウザ上で Unity の高品位なレンダリングを実現するためのソリューションです。例えば、カーコンフィギュレータや建築モデルをスマートデバイスで閲覧するニーズを想定しています。
ストリーミング技術には [WebRTC](https://webrtc.org/) というテクノロジを用いて実現しています。WebRTC の API を提供しているため、デベロッパーがカスタマイズすることで様々なソリューションを構築することが可能です。

<< gif アニメーションを貼る >>

このリポジトリでは2つのパッケージを管理します。各パッケージの README.md に、より詳細な情報を提供しています。

- [com.unity.template.renderstreming](Packages/com.unity.template.renderstreaming)
- [com.unity.webrtc](Packages/com.unity.webrtc)

## 動作環境

Unity 2019.1 で動作を確認しています。現在は Windows 64bit 環境でのみ動作を確認しています。
Unity 2018.3 は [New Input System](https://github.com/Unity-Technologies/InputSystem) に依存しているため動作しません。

### 対応ブラウザ

利用可能なブラウザについては、ブラウザ毎の WebRTC への対応状況に依存しています。

- Desktop Chrome
- Desktop Firefox
- Desktop Safari
- Android Chrome
- iOS Safai

### ライセンス

`com.unity.template.renderstreaming` のライセンス
- [LICENSE.md](Packages/com.unity.template.renderstreaming/LICENSE.md)

`com.unity.webrtc` のライセンス
- [LICENSE.md](Packages/com.unity.webrtc/LICENSE.md)
- [Third Party Notices.md](Packages/com.unity.webrtc/Third%20Party%20Notices.md)

### グラフィックボード

このソリューションは、 NVIDIA が提供するグラフィックボードに最適化されています。推奨するグラフィックボード上で動作させることで、低遅延のストリーミング配信を実現することができます。推奨グラフィックボードのリストについては NVIDIA が公開している [NVIDIA VIDEO CODEC SDK](https://developer.nvidia.com/video-encode-decode-gpu-support-matrix) のウェブページを参照してください。

## パッケージ構成

```
<root>
├── Assets
│   ├── Plugins
│   ├── Scenes
│   ├── Scripts
│   └── Tests
├── Packages
│   ├── com.unity.template.renderstreaming
│   └── com.unity.webrtc
├── ProjectSettings
├── UnityRenderStreamingPlugin
│   ├── UnityRenderStreamingPlugin
│   ├── WebRTCPlugin
│   └── unity
└── WebApp
    ├── public
    ├── src
    └── test
```

## ロードマップ

|Version|libwebrtc|Focus|
|-------|-----|-----|
|`0.1-preview`|M72|- <br>- <br>- <br>- |
|`0.2-preview`|M72|- <br>- <br>- <br>- |
|`0.3-preview`|M72|- <br>- <br>- <br>- |

## FAQ

## Contributer

- [@karasusan](https://github.com/karasusan)
- [@hiroki-o](https://github.com/hiroki-o)
- [@flame99999](https://github.com/flame99999)
- [koseyile](https://github.com/koseyile)