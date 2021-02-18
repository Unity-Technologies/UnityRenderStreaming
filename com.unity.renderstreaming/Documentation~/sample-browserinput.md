# Web Browser Input Sample

This sample demonstrates to stream the video from Unity to the web browser, and operate input events on the browser as Unity events.

you can see the list of supported browsers on [The Web Application](webapp.md) page.

## Usage

### Hardware acceleration 

Recommend enabling `Hardware Encoder support` on the `Render Streaming` inspector to improve performance using hardware acceleration.

### Procedure

1. Launch the signaling server using public mode. Please refer to [this link](webapp.md) for launching.

![Launch web server](images/launch_webserver_public_mode.png)

2. Open `WebBrowserInput` scene and specify IP address of the signaling server to the `Signaling URL` parameter on the inspector.

![Signaling url on inspector](images/webbrowserinput_sample_signaling_url_on_inspector.png)

3. Click on the Play button in Unity Editor.

4. Open your web browser and access `http://localhost`. You can see the top page. Click a `VideoPlayer Sample` link.

![Main page on the browser](images/browser_mainpage.png)

5. You can see the Unity scene on the browser, and control a camera in the Unity scene.

![Video player sample on the browser](images/browser_videoplayer.png)
