# Unity Render Streaming

- [Japanese](jp/index.md)

**Unity Render Streaming** は、ブラウザ上で Unity の高品位なレンダリングを実現するためのソリューションです。カーコンフィギュレータや建築モデルをスマートデバイスで閲覧するニーズを想定しています。
ストリーミング技術には [WebRTC](https://webrtc.org/) というテクノロジを用いて実現しています。デベロッパーがソリューションをカスタマイズすることで、様々なソリューションを構築することができます。

<img src="./images/multitouch.gif" width=500 align=center>

## Guide

* [Tutorial](en/tutorial.md)
* [Input](en/input.md)
* [Web Server](en/webserver.md)
* [FAQ](en/faq.md)

## Requirements

This version of Render Streaming is compatible with the following versions of the Unity Editor:

- 2019.1 and later (recommended)

> [!NOTE]
> <`Unity 2018.3` is not supported.>

Currently the software support `windows64` only.

バージョン1.0.0 現在、 NVIDIA が提供するグラフィックボードに最適化されています。推奨するグラフィックボード上で動作させることで、低遅延のストリーミング配信を実現することができます。推奨グラフィックボードのリストについては NVIDIA が公開している [NVIDIA VIDEO CODEC SDK](https://developer.nvidia.com/video-encode-decode-gpu-support-matrix) のウェブページを参照してください。

### Browser

- Desktop Chrome
- Desktop Firefox
- Desktop Safari
- Android Chrome
- iOS Safari