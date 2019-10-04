# WebRTC

- [English](../index.md)

WebRTC for Unity は、 [WebRTC](https://webrtc.org) を Unity で利用可能にするためのパッケージです。

## ガイド

* [チュートリアル](tutorial.md)

## インストール方法
パッケージをインストールするためには、パッケージマネージャーから WebRTC for Unity を検索しインストールします。パッケージマネージャーの利用方法は[ドキュメント](https://docs.unity3d.com/Packages/com.unity.package-manager-ui@latest/index.html)を参照してください。

<img src="../images/webrtc_package_manager.png" width=600 align=center>

## サンプル
パッケージでは以下の 3 つのサンプルを用意しています。

| シーン名       | 説明                                 |
| -------------- | ------------------------------------ |
| PeerConnection | ピアを接続する手続きを確認するシーン |
| DataChannel    | テキスト送受信を確認するシーン       |
| MediaStream    | ビデオ/音声送受信を確認するシーン    |

## 制限事項

ビデオエンコーディングに GPU ハードウェアアクセラレーションを使用しているため、NVCodec 対応のグラフィックボード上でのみ動作します。

## 更新履歴

|Date|Reason|
|---|---|
|June 21, 2019|ドキュメント公開|
