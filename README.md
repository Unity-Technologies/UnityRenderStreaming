# Unity Render Streaming

<img src="https://img.shields.io/badge/unity-2019.1-green.svg?style=flat-square" alt="unity 2019.1">

**Unity Render Streaming** is a solution that provides Unity's high quality rendering abilities via browser. It's designed to meet the needs of tasks like viewing car configurators or architectural models on mobile devices.  
This solution's streaming technology takes advantage of [WebRTC](https://webrtc.org/), and developers can even use the WebRTC API to create their own unique solutions.

<img src="Packages/com.unity.template.renderstreaming/Documentation~/images/multitouch.gif" width=500 align=center>

This repository manages 2 packages. Details can be found in the README.md for each package.

- [com.unity.template.renderstreming](Packages/com.unity.template.renderstreaming/Documentation~/index.md)
- [com.unity.webrtc](Packages/com.unity.webrtc/Documentation~/index.md)

## Operating Environment

This solution is compatible with Unity 2019.1. Currently it only runs in a Windows 64-bit environment.
Unity 2018.3 relies on the [New Input System](https://github.com/Unity-Technologies/InputSystem), so it is incompatible with this solution.

### Supported Browsers

Browser support depends on the status of each browser's WebRTC support.

- Desktop Chrome
- Desktop Firefox
- Desktop Safari
- Android Chrome
- iOS Safari

### License

`com.unity.template.renderstreaming` license
- [LICENSE.md](Packages/com.unity.template.renderstreaming/LICENSE.md)

`com.unity.webrtc` license
- [LICENSE.md](Packages/com.unity.webrtc/LICENSE.md)
- [Third Party Notices.md](Packages/com.unity.webrtc/Third%20Party%20Notices.md)

### Graphics Cards

This solution is optimised for NVIDIA graphics cards. Using a recommended graphics card will allow streaming with lower latency. See NVIDIA's [NVIDIA VIDEO CODEC SDK](https://developer.nvidia.com/video-encode-decode-gpu-support-matrix) webpage for a list of recommended graphics cards.

## Package Structure

```
<root>
├── Assets
│   ├── Plugins
│   ├── Scenes
│   ├── Scripts
│   └── Tests
├── Packages
│   ├── com.unity.template.renderstreaming
│   └── com.unity.webrtc
├── ProjectSettings
├── UnityRenderStreamingPlugin
│   ├── UnityRenderStreamingPlugin
│   ├── WebRTCPlugin
│   └── unity
└── WebApp
    ├── public
    ├── src
    └── test
```

## Roadmap

|Version|libwebrtc|Focus|
|-------|-----|-----|
|`0.1-preview`|M72|- <br>- <br>- <br>- |
|`0.2-preview`|M72|- <br>- <br>- <br>- |
|`0.3-preview`|M72|- <br>- <br>- <br>- |

## FAQ

## Contributors

- [@karasusan](https://github.com/karasusan)
- [@hiroki-o](https://github.com/hiroki-o)
- [@flame99999](https://github.com/flame99999)
- [koseyile](https://github.com/koseyile)
