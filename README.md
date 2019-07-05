# Unity Render Streaming

<img src="https://img.shields.io/badge/unity-2019.1-green.svg?style=flat-square" alt="unity 2019.1">

**Unity Render Streaming** は、ブラウザ上で Unity の高品位なレンダリングを実現するためのソリューションです。例えば、カーコンフィギュレータや建築モデルをスマートデバイスで閲覧するニーズを想定しています。
ストリーミング技術には [WebRTC](https://webrtc.org/) というテクノロジを用いて実現しています。WebRTC の API を提供しているため、デベロッパーがカスタマイズすることで様々なソリューションを構築することが可能です。

<img src="Packages/com.unity.template.renderstreaming/Documentation~/images/multitouch.gif" width=500 align=center>

このリポジトリでは 2つのパッケージを管理しています。各パッケージのドキュメントで詳細な情報を提供しています。

- [com.unity.template.renderstreming](Packages/com.unity.template.renderstreaming/Documentation~/index.md)
- [com.unity.webrtc](Packages/com.unity.webrtc/Documentation~/index.md)

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
|`1.0.0`|M72|- Hardware Encoding <br>- Signaling server <br>- HDRP sample |
|`1.1.0`||- Simulcast <br>- Multi camera<br>- MultiStream |
|`1.2.0`||- AVAPI Support<br>- Linux Support |

## FAQ

## Contributer

- [@karasusan](https://github.com/karasusan)
- [@hiroki-o](https://github.com/hiroki-o)
- [@flame99999](https://github.com/flame99999)
- [koseyile](https://github.com/koseyile)