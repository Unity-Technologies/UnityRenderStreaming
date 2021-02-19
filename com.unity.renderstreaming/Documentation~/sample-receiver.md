# Receiver Sample

This sample shows that displaying a video streamed from another Unity.

This sample can use in combination with other samples below.

- [Broadcast](sample-broadcast.md)
- [Bidirectional](sample-bidirectional.md)

> [!NOTE]
> In this version, it was checked on only iOS devices. Android and other devices are not supported by this sample.

> [!NOTE]
> If no codec is available on both sender and receiver, the video can not stream. Please note that the compatibility of codecs is different corresponding to platforms and graphics API.

![ReceiveVideo Sample](images/renderstreaming_receivevideosample.png)

## Usage

### Hardware acceleration 

Recommend enabling `Hardware Encoder support` on the `Render Streaming` inspector to improve performance using hardware acceleration.

### Procedure

This procedure is for using it with [Broadcast](sample-broadcast.md) sample.

1. Launch the signaling server using public mode. Please refer to [this link](webapp.md) for launching.










This is a sample for receiving VideoTrack of MediaStream. 
Click `Start` to create a new `RTCPeerConnection` and send the Offer to server.
You can also use any `ConnectionId`. In that case, enter the `ConnectionId` in the `InputField`.
This scene uses `ReceiveVideoViewer` component to receive a VideoTrack of MediaStream.
Please see [Components settings](components.md) page for details.