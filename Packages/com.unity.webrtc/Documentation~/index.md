# WebRTC for Unity

- [Japanese](jp/index.md)

WebRTC for Unity は、 [WebRTC](https://webrtc.org) を Unity で利用可能にするためのパッケージです。

## Guide

- [Tutorial](en/tutorial.md)

## Install

パッケージをインストールするためには、パッケージマネージャから `WebRTC` を検索しインストールします。パッケージマネージャの利用方法は[ドキュメント](https://docs.unity3d.com/Packages/com.unity.package-manager-ui@latest/index.html)を参照してください。

<img src="./images/webrtc_package_manager.png" width=600 align=center>

## Sample
パッケージでは以下の3つのサンプルを用意しています。
| シーン名        | 説明                                 |
|----------------|-------------------------------------|
| PeerConnection | ピアを接続する手続きを確認するシーン     |
| DataChannel    | テキスト送受信を確認するシーン          |
| MediaStream    | ビデオ/音声送受信を確認するシーン       |

## Limitation

ビデオエンコーディングにGPUハードウェアアクセラレーションを使用しているため、NVCodec 対応のグラフィックボードにのみ動作しています。

## 更新履歴

|Date|Reason|
|---|---|
|June 21, 2019|ドキュメント公開|