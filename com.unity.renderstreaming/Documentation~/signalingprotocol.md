# Signaling Protocol

**The Unity Render Streaming** package provides two different signaling protocols as examples.

- `HttpSignaling`
- `WebSocketSignaling`

In the example, the schema given to `URL Signaling` is used to determine which protocol to use.

![Render Streaming backend](images/websocket_signaling_inspector.png)

If it starts with `http`, `HttpSignaling` is used. If it starts with `ws`, `WebSocketSignaling` is used.

## HttpSignaling
Signaling is handled by HTTP Request.
The signalling server is polled at specified intervals to obtain the Offer and Candidate of the difference from the last time.

## WebSocketSignaling
Signaling is handled by WebSocket.
When the signaling server receives the Offer or Candidate, the server distributes it to the connected clients.
> [!WARNING]
> WebSocket does not work in iOS Safari on servers that use self-signed certificates.
> If you want to verify the behavior of WebSocket signaling in iOS Safari, use a certificate issued by a trusted certification authority. Or try signaling with HTTP.