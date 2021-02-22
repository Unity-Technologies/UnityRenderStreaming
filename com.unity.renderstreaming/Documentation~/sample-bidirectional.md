# Bidirectional Sample

This is a sample of bi-directional(send/receive) each other's video.

It works with Unity-Unity or Unity-Browser combination.

> [!NOTE]
> If no codec is available on both sender and receiver, the video can not stream. Please note that the compatibility of codecs is different corresponding to platforms and graphics API.

## Usage

### Hardware acceleration 

Recommend enabling `Hardware Encoder support` on the `Render Streaming` inspector to improve performance using hardware acceleration.

### Procedure (Unity-Unity)

1. Launch the signaling server using private mode. Please refer to [this link](webapp.md) for launching.

> [!NOTE]
> This sample does not work on signaling server in public mode.

![Launch web server](images/launch_webserver_private_mode.png)

2. Open `Bidirectional` scene and specify IP address of the signaling server to the `Signaling URL` parameter on the inspector.

3. If you need, change device index on `WebCamStremer` compornent.

![Change Device Index](images/change_device_index_on_webcamstremr.png)

4. Build and run application. After runnning application click `SetUp` button. (ConnectionID is autofill.)

![Bidirectional Sample Setup](images/sample_bidirectional_setup.png)

> [!NOTE]
> If use on iOS/macOS platform, you need set `Camera Usage Description` on Player Settings.

![Set Camera Usage Description](images/sample_bidirectional_camerausagedescription.png)

5. Enter same ConnectionID on the application in Unity Editor.

![Enter Same ID](images/sample_bidirectional_entersameid.png)

5. Press the `SetUp` button in Unity Editor.

6. You can see streaming video each other's. (Of course, it also works with Unity applications.)

![Bidirectional Streaming](images/sample_bidirectional_streaming.png)

### Procedure (Unity-Broweser)

1.~3. steps same as Unity-Unity Procedure. 

4. Open your web browser and access `http://localhost`. You can see the top page. Click a `Bidirectional Sample` link.

![Browser Bidirectional Sample Link](images/browser_mainpage_bidirectional.png)

5. Enter same ConnectionID on the Unity Application to Browser text area.

![Enter Same ID on browser](images/sample_bidirectional_entersameid_browser.png)

6. Click `StartVideo` button, after that click `SetUp` button in Browser.

![Click StartVideo Button](images/browser_bidirectional_sample_startvideo.png)

7. You can see streaming video each other's.

![Bidirectional Streaming with browser](images/sample_bidirectional_streaming_with_browser.png)