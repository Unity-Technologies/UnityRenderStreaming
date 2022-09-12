# FAQ

## Errors

### Video stream not showing in browser

The browser version might be too old. Make sure that the latest version is installed. Or firewall settings can also cause the issue, please try to turn it off once.

Please check if there is any error log on the browser console and Unity debug console.

### Web app doesn't run on Safari browser

Check the web server's [startup options](webapp.md). In **Safari** and **iOS Safari**, WebRTC features cannot be used with **http**. Instead, **https** must be used.
Also, **iOS Safari** does not support WebSocket signaling if the server uses a self-signed certificate. Please check [here](signaling-type.md) for more information.

### Unity's initial startup fails

Confirm you're using a `Graphics API` and compare [requirements](https://docs.unity3d.com/Packages/com.unity.webrtc@latest) of `com.unity.webrtc`.
If you use the hardware encoder, Make sure you're using a graphics card that supports **Nvcodec**. Also confirm that the latest graphics driver version is installed. See also: `com.unity.webrtc` documentation.

### Web server fails to run

Make sure the port isn't being used by another service.

### Unity UI can't be operated through a browser.

It is only possible to operate when the focus is on the running application.
On [This page](browser-input.md#using-unity-ui), you can see more info.

## Issues with the environment

### Which versions of Unity can be used?

**Unity 2020.3** or higher.

### Which OS can be used?

**Windows10 x64**, **Linux x64**, **macOS** and **iOS** **Android**.

**UWP** and **WebGL** are not supported.

### Are there any hardware constraints?

On Windows and Linux, the hardware codec uses an NVIDIA graphics card. See the [NVIDIA site](https://developer.nvidia.com/video-encode-decode-gpu-support-matrix) for information on graphics cards that support NVCodec.

### Which browsers are supported?

See [this page](https://caniuse.com/#search=webrtc).

## Questions about features

### Is it possible to broadcast to multiple devices?

Yes. However, the more devices being transmitted to, the greater the burden on the transmitting device. For example, on a new desktop PC it's possible to transmit 720p video to about 5 devices, but anything more than that will require using an [SFU](https://webrtcglossary.com/sfu/) broadcast server.

### Is simulcast supported?

Currently, we do not support simulcast.

### Can you broadcast the feed from multiple cameras?

Yes. If you use the hardware encoder, make sure to check the limitation of the number of cameras used at the same time. See the NVIDIA [document](https://developer.nvidia.com/video-encode-decode-gpu-support-matrix).

### How do you increase the video quality?

You can set the resolution of the frame on the inspector window of [video stream components](video-streaming.md). Keep in mind that video quality depends on the quality of the network.

## Questions about support

### Technical support from Unity can be gotten?

It is available to ask questions on the page of [Unity forum](https://forum.unity.com/forums/unity-render-streaming.413). Also please report on [Github Issues](https://github.com/Unity-Technologies/UnityRenderStreaming/issues) for reporting bugs or requesting features.

### Is it available to deploy Unity to cloud service like GCP as a rendering server?

Unity is not supported using for rendering server.
