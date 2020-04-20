# Signaling Protocol

**The Unity Render Streaming** package provides two different signaling protocols as examples.

- HttpSignaling
- WebSocketSignaling

In the example, the schema given to `URL Signaling` is used to determine which protocol to use.

![Render Streaming backend](../images/websocket_signaling_inspector.png)

If it starts with `http`, HttpSignaling is used. If it starts with `ws`, WebSocketSignaling is used.

## HttpSignaling
Signaling is handled by Http Request.
The signalling server is polled at specified intervals to obtain the Offer and Candidate of the difference from the last time.

## WebSocketSignaling
Signaling is handled by WebSocket.
When the signaling server receives the Offer or Candidate, the server distributes it to the connected clients.

## Proprietary Signaling Class
Both of the signaling classes implement `ISignaling`.
If you want to create your own signaling class, you can inherit `ISignaling` to use it directly from the UnityRenderStreaming class.
The following is a description of each method.

```
void Start();
```
- A method to call when starting signaling.
- In the sample, we are establishing a session with the server for each protocol.

```
void Stop();
```
A method to call when you want to stop signaling.
In the sample, we are terminating the session with the server for each protocol.

```
public delegate void OnOfferHandler(ISignaling signaling, DescData e);
event OnOfferHandler OnOffer;
```
- This is a delegate that registers the process to be performed when a new Offer is received by signaling.

```
public delegate void OnAnswerHandler(ISignaling signaling, DescData e);
event OnAnswerHandler OnAnswer;
```
- This is a delegate that registers the process to be performed when a new Answer is received by signaling.
- In the current version, it is not used in Unity because it does not receive Answers.

```
public delegate void OnIceCandidateHandler(ISignaling signaling, CandidateData e);
event OnIceCandidateHandler OnIceCandidate;
```
- This is a delegate that registers the process to be performed when it receives a new Candidate by signaling.

```
void SendOffer();
```
- A method to call when sending an Offer in signaling.
- In the current version, it is not implemented in the sample because it is not possible to send Offer from Unity.

```
void SendAnswer(string connectionId, RTCSessionDescription answer);
```
- A method to call when sending an Answer in signaling.

```
void SendCandidate(string connectionId, RTCIceCandidate candidate);
```
- A method to call when sending a Candidate in signaling.
