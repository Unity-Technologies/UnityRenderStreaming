# Render Streaming 概要

Render Streaming のシステムの概観について説明します。

### システム構成

Render Streaming のシステムは以下の３つで構成されます。

- Unity （エディタもしくはアプリケーション）
- Web サーバ
- Web ブラウザ 

<img src="../images/renderstreaming_overview.png" width=500 align=center>

Render Streaming では、**Unity** と **Web ブラウザ** が P2P ネットワークを構成し、UDP/IP を利用して通信を行います。**Web サーバ** は Web ブラウザと Unity の情報のやり取りを仲介します。この情報のやり取りのことをシグナリングと呼びます。

> [!NOTE]
> 現在、以下の機能に対応していません。
>
> - ブラウザから Unity へのビデオ送信
> - ブラウザから Unity へのオーディオ送信

### シグナリング処理の流れ

シグナリング処理の流れを説明します。

 <img src="../images/renderstreaming_sequence.png" width=600 align=center>

1. Web ブラウザから Web サーバに **Offer SDP** を送信します。
2. Unity から Web サーバに問い合わせて、未処理の **Offer SDP** を取得します。
3. Unity から Web サーバに **Answer SDP** を送信します。
4. Web ブラウザから Web サーバに問い合わせて、未処理の **Answer SDP** を取得します。
5. Web ブラウザから Web サーバに **ICE Candidate** を送信します。
6. Unity から Web サーバに問い合わせて、未処理の **ICE Candidate** を取得します。
7. Unity から Web サーバに **ICE Candidate** を送信します。
8. Web ブラウザから Web サーバに問い合わせて、未処理の **ICE Candidate** を取得します。

##パッケージ構成 

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

### ウェブアプリケーション

クライアントサイド及びサーバサイドのサンプルを含みます。また、シグナリングサーバの機能を提供しています。ウェブアプリケーションの詳細については[ドキュメント](webserver.md)を参照してください。