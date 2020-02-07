# 概要

Render Streaming のシステムの概観について説明します。

### システム構成

Render Streaming のシステムは以下の 3 つで構成されます。

- **Unity** （エディターもしくはアプリケーション）
- **Web サーバー**
- **Web ブラウザー**

![Render Streaming overview](../images/renderstreaming_overview.png)

Unity Render Streaming では、**Unity** と **Web ブラウザー** が P2P ネットワークを構成し、UDP/IP を利用して通信を行います。**Web サーバー** は Web ブラウザーと Unity の情報のやり取りを仲介します。この情報のやり取りのことをシグナリングと呼びます。

> [!WARNING]
> 現在、以下の機能に対応していません。
>
> - ブラウザーから Unity へのビデオ送信
> - ブラウザーから Unity へのオーディオ送信

### シグナリング処理の流れ

シグナリング処理の流れを説明します。

![Render Streaming sequence](../images/renderstreaming_sequence.png)

1. Web ブラウザーから Web サーバーに **Offer SDP** を送信します。
2. Unity から Web サーバーに問い合わせて、未処理の **Offer SDP** を取得します。
3. Unity から Web サーバーに **Answer SDP** を送信します。
4. Web ブラウザーから Web サーバーに問い合わせて、未処理の **Answer SDP** を取得します。
5. Web ブラウザーから Web サーバーに **ICE Candidate** を送信します。
6. Unity から Web サーバーに問い合わせて、未処理の **ICE Candidate** を取得します。
7. Unity から Web サーバーに **ICE Candidate** を送信します。
8. Web ブラウザーから Web サーバーに問い合わせて、未処理の **ICE Candidate** を取得します。

### パッケージ構成

Render Steraming パッケージには以下の内容が含まれています。

- **WebRTC パッケージ**
- **サンプルコード / アセット**
- **ウェブアプリケーション**

![Package Render Streaming](../images/package_renderstreaming.png)

### WebRTC パッケージ

WebRTC の C# API を提供します。
Unity Render Streaming パッケージとは独立したパッケージとして管理されています。
詳細については、 [WebRTC パッケージ](https://docs.unity3d.com/Packages/com.unity.webrtc@latest/manual/jp/index.html) のドキュメントを参照してください。

![WebRTC package manager](../images/webrtc_package_manager.png)

### サンプルコード / アセット

HDRP/URP テンプレートのサンプルに、 Unity Render Streaming を組み込んだプロジェクトです。

### Web アプリケーション

クライアントサイド及びサーバーサイドのサンプルを含みます。また、シグナリングサーバーの機能を提供しています。Web アプリケーションの詳細については[ドキュメント](webserver.md)を参照してください。
