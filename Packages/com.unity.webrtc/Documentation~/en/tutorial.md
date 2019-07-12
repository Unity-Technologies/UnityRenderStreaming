# Tutorial

This tutorial will cover the basics of using the WebRTC package.


### Adding a Namespace

The namespace specifies `Unity.WebRTC`.

```CSharp
using UnityEngine;
using Unity.WebRTC;
```

### Initialization

Call `WebRTC.Initialize()` to initialize and use `WebRTC`. Call `WebRTC.Finalize()` when finished.

```CSharp
public class MyPlayerScript : MonoBehaviour
{
    private void Awake()
    {
        // Initialize WebRTC
        WebRTC.Initialize();
    }
}
```

### Creating a local peer

Create a local peer and get `RTCDataChannel`. Use `RTCDataChannel` to enable binary data transmission. Register `OnOpen` and `OnClose` callbacks to run a process when `RTCDataChannel` starts or finishes. Set the `OnMessage` callback to receive messages.

```CSharp
    // Create local peer
    var localConnection = new RTCPeerConnection();
    var sendChannel = localConnection.CreateDataChannel("sendChannel");
    channel.OnOpen = handleSendChannelStatusChange;
    channel.OnClose = handleSendChannelStatusChange;
```

### Creating a remote peer

Create a remote peer and set the `OnDataChannel` callback.

```CSharp
    // Create remote peer
    var remoteConnection = new RTCPeerConnection();
    remoteConnection.OnDataChannel = ReceiveChannelCallback;
```

### Register potential communication paths

An ICE (Interactive Connectivity Establishment) exchange is required to establish a peer connection. Once the potential communication paths for all peers have been discovered, `OnIceCandidate` is called. Use callbacks to call `AddIceCandidate` on each peer to register potential paths.


```CSharp
localConnection.OnIceCandidate = e => { !string.IsNullOrEmpty(e.candidate)
        || remoteConnection.AddIceCandidate(ref e); }

remoteConnection.OnIceCandidate = e => { !string.IsNullOrEmpty(e.candidate)
        || localConnection.AddIceCandidate(ref e); }

```

### The Signaling Process

SDP exchanges happen between peers. `CreateOffer` creates the initial Offer SDP. After getting the Offer SDP, both the local and remote peers set the SDP. Be careful not to mix up `SetLocalDescription` and `SetRemoteDescription` during this exchange. 

Once the Offer SDP is set, call `CreateAnswer` to create an Answer SDP. Like the Offer SDP, the Answer SDP is set on both the local and remote peers.

```csharp
var op1 = localConnection.CreateOffer();
yield return op1;
var op2 = localConnection.SetLocalDescription(ref op1.desc);
yield return op2;
var op3 = remoteConnection.SetRemoteDescription(ref op1.desc);
yield return op3;
var op4 = remoteConnection.CreateAnswer();
yield return op4;
var op5 = remoteConnection.setLocalDescription(op4.desc);
yield return op5;
var op6 = localConnection.setRemoteDescription(op4.desc);
yield return op6;
```

### Check the ICE Connection Status

When SDP exchanges happen between peers, ICE exchanges begin. Use the `OnIceConnectionChange` callback to check the ICE connection status.

```CSharp
localConnection.OnIceConnectionChange = state => {
    Debug.Log(state);
}
```

### The Data Channel Connection

When the ICE exchange is finished, `OnDataChannel` is called and a one-way peer Data Channel is created.
Register the `OnMessage` callback and describe the procdure for when a message is received.

```CSharp
RTCDataChannel receiveChannel;
void ReceiveChannelCallback(RTCDataChannel channel) 
{
    receiveChannel = channel;
    receiveChannel.OnMessage = HandleReceiveMessage;  
}
```

### Sending Messages

When both peers' `RTCDataChannel` is open, it's possible to exchange messages. `string` or `byte[]` message types can be sent. 

```csharp
void SendMessage(string message)
{
  sendChannel.Send(message);
}

void SendBinary(byte[] bytes)
{
  sendChannel.Send(bytes);
}
```

### Receiving Messages

When a message is received, the callback registered to `OnMessage` is called. `byte[]` type messages can be received, and when treated like character strings they are converted as shown below.

```csharp
void HandleReceiveMessage(byte[] bytes)
{
  var message = System.Text.Encoding.UTF8.GetString(bytes);
  Debug.Log(message);
}
```

### The End Process

When finished, `Close()` must be called for `RTCDataChannel` and `RTCPeerConnection`. Finally, after the object is discarded, call `WebRTC.Finalize()`.

```csharp
private void OnDestroy()
{
  sendChannel.Close();
  receiveChannel.Close();
  
  localConnection.Close();
  remoteConnection.Close();
  
  WebRTC.Finalize();
}
```

### Video Streaming

Use the `Camera`'s `CaptureStream()` to use `MediaStream` in order to capture the video stream. 
