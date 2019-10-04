# Unity Render Streaming

- [English](../index.md)

**Unity Render Streaming** は、ブラウザー上で Unity の高品位なレンダリングを実現するためのソリューションです。カーコンフィギュレーターや建築モデルをスマートデバイスで閲覧するニーズを想定しています。
ストリーミング技術には [WebRTC](https://webrtc.org/) というテクノロジを用いて実現しています。デベロッパーがソリューションをカスタマイズすることで、様々なソリューションを構築することができます。

<img src="../images/multitouch.gif" width=500 align=center>

## ガイド

* [概要](overview.md)
* [チュートリアル](tutorial.md)
* [ブラウザ入力](input.md)
* [Web サーバ](webserver.md)
* [Render Streaming リファレンス](class-renderstreaming.md)
* [FAQ](faq.md)

## 動作環境

- このバージョンの RenderStreaming は次のバージョンの Unity エディターに対応しています：
  - 2019.1

> [!NOTE]
> <`Unity 2018.3` はサポートされていません。>

> [!NOTE]
> <`Unity 2019.2` は HDRP 5.x がサポートされないため、描画が正しく動作しません。>

- 現在は `windows64` 環境でのみ動作を確認しています。

- Graphics API は `DirectX11` のみサポートしています。

- Scripting Backend の `IL2CPP` は現在のところサポートされていません。

### グラフィックスカード

バージョン 1.0.0 現在、 NVIDIA が提供するグラフィックボードに最適化されています。推奨するグラフィックボード上で動作させることで、低遅延のストリーミング配信を実現することができます。推奨グラフィックボードのリストについては NVIDIA が公開している [NVIDIA VIDEO CODEC SDK](https://developer.nvidia.com/video-encode-decode-gpu-support-matrix) のウェブページを参照してください。

Graphics API は `Direct3D11` のみサポートしています。

### 対応ブラウザー

- Desktop Chrome
- Desktop Firefox
- Desktop Safari
- Android Chrome
- iOS Safari
