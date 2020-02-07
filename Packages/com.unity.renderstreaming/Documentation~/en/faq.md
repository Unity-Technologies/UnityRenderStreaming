# FAQ

## Errors

### Video stream not showing in browser

The browser version might be too old. Make sure that the latest version is installed.

### Web app doesn't run on iOS Safari 

Check the web server's [startup options](webserver.md). In **Safari** and **iOS Safari**, WebRTC features cannot be used with **http**. Instead, **https** must be used.

### Unity's initial startup fails

Make sure you're using a graphics card that supports **Nvcodec**. Also confirm that the latest graphics driver version is installed. See also: `com.unity.webrtc` documentation.

And currently, this package supports only `Direct3D11`. If you select other API, the initialization process will fail. 

### Web server fails to run 

Make sure the port isn't being used by another service. 

## Issues with the environment

### Which versions of Unity can be used?

Unity 2019.1 and later.

### Which OS can be used?

Windows10 x64. Support for other OS' will be provided later.

### Are there any hardware constraints?

The hardware code uses NVCodec, so an NVIDIA graphics card is required. See the NVIDIA [site](https://developer.nvidia.com/video-encode-decode-gpu-support-matrix) for information on graphics cards that support NVCodec.

### Which browsers are supported?

Any browser, as long as it supports WebRTC.

## Questions about features

### Is it possible to broadcast to multiple devices?

Yes. However, the more devices being transmitted to, the greater the burden on the transmitting device. For example, on a new desktop PC it's possible to transmit 720p video to about 5 devices, but anything more than that will require using an [SFU](https://webrtcglossary.com/sfu/) broadcast server.

### Is simulcast supported?

Currently, version 1.0 does not support simulcast.

### Can you broadcast the feed from multiple cameras?

Currently, version 1.0 cannot.

### How do you increase the video quality?

Video quality depends on the quality of the network. 

### Is Realtime-Raytracing supported? 

Currently, version 1.0 cannot.
