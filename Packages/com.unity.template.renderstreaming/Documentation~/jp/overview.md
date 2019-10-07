# Render Streaming 概要

Render Streaming のシステムの概観について説明します。

### システム構成

Render Streaming のシステムは以下の 3 つで構成されます。

- Unity （エディターもしくはアプリケーション）
- Web サーバー
- Web ブラウザー

<img src="../images/renderstreaming_overview.png" width=500 align=center>

Render Streaming では、**Unity** と **Web ブラウザー** が P2P ネットワークを構成し、UDP/IP を利用して通信を行います。**Web サーバー** は Web ブラウザーと Unity の情報のやり取りを仲介します。この情報のやり取りのことをシグナリングと呼びます。

> [!NOTE]
> 現在、以下の機能に対応していません。
>
> - ブラウザーから Unity へのビデオ送信
> - ブラウザーから Unity へのオーディオ送信

### シグナリング処理の流れ

シグナリング処理の流れを説明します。

 <img src="../images/renderstreaming_sequence.png" width=600 align=center>

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

- WebRTC パッケージ
- サンプルコード / アセット
- ウェブアプリケーション

<img src="../images/package_renderstreaming.png" width=300 align=center>

### WebRTC パッケージ

WebRTC の C# API を提供します。  API の詳細は `com.unity.webrtc` のドキュメントを参照してください。

<img src="../images/webrtc_package_manager.png" width=500 align=center>

### サンプルコード / アセット

HDRP テンプレートのサンプルに Render Streaming を組み込んだテンプレートプロジェクトです。

### Web アプリケーション

クライアントサイド及びサーバーサイドのサンプルを含みます。また、シグナリングサーバーの機能を提供しています。Web アプリケーションの詳細については[ドキュメント](webserver.md)を参照してください。
