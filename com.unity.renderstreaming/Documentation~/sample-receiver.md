# Receiver Sample

This sample shows that displaying a video streamed from another Unity.

This sample can use in combination with other samples below.

- [Broadcast](sample-broadcast.md)
- [Bidirectional](sample-bidirectional.md)

![ReceiveVideo Sample](images/renderstreaming_receivevideosample.png)

> [!NOTE]
> In this version, it was checked on only iOS devices. Android and other devices are not supported by this sample.

> [!NOTE]
> If no codec is available on both sender and receiver, the video can not stream. Please note that the compatibility of codecs is different corresponding to platforms and graphics API.

## Usage

### Hardware acceleration 

Recommend enabling `Hardware Encoder support` on the `Render Streaming` inspector to improve performance using hardware acceleration.

### Procedure

This procedure is for using it with the [Broadcast](sample-broadcast.md) sample.

1) Launch the signaling server using public mode. Please refer to [this link](webapp.md) for launching.

![Launch web server](images/launch_webserver_public_mode.png)

2) Build a `Receiver` scene in Unity Editor, and launch an application.

3) Open `Broadcast` scene.

![Open Broadcast scene](images/open_broadcast_scene.png)

4) Specify IP address of the signaling server to the `Signaling URL` parameter on the inspector.

![Signaling url on inspector](images/broadcast_sample_signaling.png)

5) Click on the Play button in Unity Editor.

6) Operate the receiver and check the streaming video/audio correctly.