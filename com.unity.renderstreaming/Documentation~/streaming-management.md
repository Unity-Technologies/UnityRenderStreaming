# Streaming Management

Unity Render Streaming automatically creates and destroys streams as needed. It also allows the developer to customize the mechanism for controlling the stream. For a description of components related to individual streams, see the section on components for streaming.

## [`RenderStreaming`](../api/Unity.RenderStreaming.RenderStreaming.html) component

**Render Streaming** includes the following features.

- Communication with signaling servers
- Streaming input from cameras 
- Associating browser input with events

![Render Streaming Inspector](images/renderstreaming_inspector.png)

### Properties

| Parameter | Description | Default |
| --------- | ----------- | ------- |
| **Signaling Type** | Class type of the signaling (See [About Signaling Type](signaling-type.md)). | `WebSocketSignaling` |
| **Signaling URL** | Set the signaling server URL <br>If you use WebSocket as a signaling protocol, you should specify a URL starting with `ws` or `wss`. | `ws://localhost` |
| **Ice Servers** | Set a list of STUN/TURN servers. (See [TURN Server](turnserver.md)) | |
| *Urls* | Set URLs of STUN/TURN servers. | `[stun:stun.l.google.com:19302\]` |
| *Username* | The username to use when logging into the TURN server. | |
| *Credencial* | The credential to use when logging into the TURN server. | |
| *Credencial Type* | This attribute specifies what kind of `credential` is to be used when connecting to the TURN server. | `Password` |
| **Interval** | Set the polling frequency (in seconds) to the signaling server <br>Valid only if you are using Http as the signaling protocol. | 5 (sec) |
| **Hardware Encoder Support** | Enable the checkbox to make this video encoding replace values that use a [hardware encoder](index.md#hardware-encoder). | `False` |
| **Handlers** | List of handlers of signaling process. | |
| **Run On Awake** | Automatically started when called `Awake` method. | `True` |

## [`SingleConnection`](../api/Unity.RenderStreaming.SingleConnection.html) component

This component lists components that stream media or data via a single peer connection.

![Single Connection inspector](images/singleconnection_inspector.png)

### Properties

| Parameter | Description | Default |
| --------- | ----------- | ------- |
| **Streams** | List of streams | [] |

## [`Broadcast`](../api/Unity.RenderStreaming.SingleConnection.html) component

This component lists components that stream media or data via multiple peer connections.

![Broadcast inspector](images/broadcast_inspector.png)

### Properties

| Parameter | Description | Default |
| --------- | ----------- | ------- |
| **Streams** | List of streams | [] |
