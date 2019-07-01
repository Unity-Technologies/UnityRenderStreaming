# WebRTC

- [English](../index.md)

WebRTC パッケージは、 Google が開発している [WebRTC テクノロジ](https://webrtc.org)を Unity で利用可能にするためのパッケージです。

## ガイド

* [WebRTC](index.md)
* [チュートリアル](tutorial.md)

## インストール方法
パッケージをインストールするためには、パッケージマネージャから WebRTC for Unity を検索しインストールします。パッケージマネージャの利用方法は[ドキュメント](https://docs.unity3d.com/Packages/com.unity.package-manager-ui@latest/index.html)を参照してください。

![](../images/webrtc_package_manager.png) <br/>

## サンプル
パッケージでは以下の3つのサンプルを用意しています。
- PeerConnection
- DataChannel
- MediaStream

| シーン名        | 説明                                 |
|----------------|-------------------------------------|
| PeerConnection | 2つのピアを確立させる手続き確認用のシーン |
| DataChannel    | テキスト送受信の確認用のシーン          |
| MediaStream    | ビデオ/音声送受信の確認用のシーン       |

## 制限事項

ビデオエンコーディングにGPUハードウェアアクセラレーションを使用しているため、NVCodec 対応のグラフィックボードにのみ動作します。

## 更新履歴

|Date|Reason|
|---|---|
|June 21, 2019|ドキュメント公開|