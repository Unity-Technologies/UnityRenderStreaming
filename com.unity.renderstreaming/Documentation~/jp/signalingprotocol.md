# Signaling Protocol

**Unity Render Streaming** のパッケージにおいては2種類のシグナリングプロトコルをサンプルとして提供しています。

- HttpSignaling
- WebSocketSignaling

サンプルにおいては、 `URL Signaling` に指定されるスキーマからどちらのプロトコルを使用するか判定しています。

![Render Streaming backend](../images/renderstreaming_inspector.png)

`http`始まりの場合は、HttpSignaling。 `ws`始まりの場合は、WebSocketSignalingを使用します。

## HttpSignaling
Httpリクエストによって、シグナリングを行います。
シグナリングサーバーへは指定間隔でポーリングを行い、前回との差分のOfferとCandidateを取得します。

## WebSocketSignaling
WebSocketによって、シグナリングを行います。
メッセージのタイプごとにシグナリングの処理を行います。

## 独自のシグナリングクラス
どちらのシグナリングクラスも `ISignaling` を実装しています。
独自のシグナリングクラスを作成する場合は、 `ISignaling` を継承すると UnityRenderStreamingクラスからそのまま差し替えて使用することができます。


