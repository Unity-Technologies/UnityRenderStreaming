# Multiplay Sample

The `Multiplay` sample can stream video and audio to multi Unity applications.

## Usage

### Hardware acceleration

Recommend enabling `Hardware Encoder support` on the `Render Streaming` inspector to improve performance using hardware acceleration.

### Procedure

1) Launch the signaling server using public mode. Please refer to [this link](webapp.md) for launching.

![Launch web server](images/launch_webserver_public_mode.png)

2) Build a scene as a receiver (ex. [`Receiver`](sample-receiver.md) scene) in Unity Editor, and launch an application.

3) Open `Menu` scene and click `Multiplay` button. 

![Open Multiplay scene](images/open_multiplay_scene.png)

4) Select **host** and click "Play" button.

![Open Multiplay scene](images/sample_multiplay_select_host.png)

5) Click on the Play button in Unity Editor.

6) Operate the receiver and check the streaming video/audio correctly.
