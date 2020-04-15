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
シグナリングサーバーがOfferやCandidateを受け取ったタイミングで、接続されているクライアントに配信します。

## 独自のシグナリングクラス
どちらのシグナリングクラスも `ISignaling` を実装しています。
独自のシグナリングクラスを作成する場合は、 `ISignaling` を継承すると UnityRenderStreamingクラスからそのまま差し替えて使用することができます。
以下に各メソッドの説明します。

```
void Start();
```
- シグナリングを開始する際に呼び出すメソッドです。
- サンプルでは、各プロトコルにおけるサーバーとのセッション確立を行っています。

```
void Stop();
```
- シグナリングを終了する際に呼び出すメソッドです。
- サンプルでは、各プロトコルにおけるサーバーとのセッション終了を行っています。

```
public delegate void OnOfferHandler(ISignaling signaling, DescData e);
event OnOfferHandler OnOffer;
```
- シグナリングによって新しいOfferを受け取ったときに実行する処理を登録するデリゲートです。

```
public delegate void OnAnswerHandler(ISignaling signaling, DescData e);
event OnAnswerHandler OnAnswer;
```
- シグナリングによって新しいAnswerを受け取ったときに実行する処理を登録するデリゲートです。
- 現在のバージョンでは、UnityにおいてAnswerを受け取る場合がないため使用されていません。

```
public delegate void OnIceCandidateHandler(ISignaling signaling, CandidateData e);
event OnIceCandidateHandler OnIceCandidate;
```
- シグナリングによって新しいCadicateを受け取ったときに実行する処理を登録するデリゲートです。

```
void SendOffer();
```
- シグナリングでOfferを送る際に呼び出すメソッドです。
- 現在のバージョンではUnityからOfferを送ることができないため、サンプルでは実装されていません。

```
void SendAnswer(string connectionId, RTCSessionDescription answer);
```
- シグナリングでAnswerを送る際に呼び出すメソッドです。

```
void SendCandidate(string connectionId, RTCIceCandidate candidate);
```
- シグナリングでCandidateを送る際に呼び出すメソッドです。
