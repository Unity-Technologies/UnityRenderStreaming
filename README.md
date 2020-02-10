# Unity Render Streaming

<img src="https://img.shields.io/badge/unity-2019.3-green.svg?style=flat-square" alt="unity 2019.3">

**Unity Render Streaming** is a solution that provides Unity's high quality rendering abilities via browser. It's designed to meet the needs of tasks like viewing car configurators or architectural models on mobile devices.  
This solution's streaming technology takes advantage of [WebRTC](https://webrtc.org/), and developers can even use the [WebRTC package](https://github.com/Unity-Technologies/com.unity.webrtc) to create their own unique solutions. 

<img src="Packages/com.unity.renderstreaming/Documentation~/images/browser_hdrpscene.png" width=500 align=center>

This repository manages 2 packages. Details can be found in the `README.md` for each package.

- [com.unity.template.renderstreaming](Packages/com.unity.template.renderstreaming/README.md)
- [com.unity.renderstreaming](Packages/com.unity.renderstreaming/README.md)

## Branch

| Branch                                                                                         | Status         |
| ---------------------------------------------------------------------------------------------- | -------------- |
| [release/1.2.0](https://github.com/Unity-Technologies/UnityRenderStreaming/tree/release/1.2.0) | Stable         |
| [release/2.0.0](https://github.com/Unity-Technologies/UnityRenderStreaming/tree/release/1.2.0) | In development |



## Operating Environment

This solution is compatible with `Unity 2019.3`.
`Unity 2018.3` relies on the [New Input System](https://github.com/Unity-Technologies/InputSystem), so it is incompatible with this solution.

### Supported Browsers

Browser support depends on the status of each browser's WebRTC support.

- Desktop Chrome
- Desktop Firefox
- Desktop Safari
- Desktop Microsoft Edge (Chrominium-based)
- Android Chrome
- iOS Safari

### License

- `com.unity.renderstreaming` -  [LICENSE.md](Packages/com.unity.renderstreaming/LICENSE.md)

- `com.unity.template.renderstreaming` -  [LICENSE.md](Packages/com.unity.template.renderstreaming/LICENSE.md)

## Package Structure

```
<root>
├── Assets
│   ├── Plugins
│   ├── Scenes
│   ├── Scripts
│   └── Tests
├── EmptyProjectForTest~
├── Packages
│   ├── com.unity.renderstreaming
│   └── com.unity.template.renderstreaming
├── ProjectSettings
├── Plugin
│   ├── WebRTCPlugin
│   └── unity
├── RenderStreamingSampleSrc~
│   └── RS_HDRPSampleSrc_1.x
└── WebApp
    ├── public
    ├── src
    └── test
```

## Roadmap

| Version | libwebrtc version                                                            | Focus                                                |
| ------- | ---------------------------------------------------------------------------- | ---------------------------------------------------  |
| `1.0`   |[M72](https://groups.google.com/d/msg/discuss-webrtc/3h4y0fimHwg/j6G4dTVvCAAJ)| - First release                                      |
| `2.0`   |[M80](https://groups.google.com/d/msg/discuss-webrtc/Ozvbd0p7Q1Y/M4WN2cRKCwAJ)| - Multi camera <br>- DirectX12 (DXR) Support         |
| `2.1`   |                                                                              | - AMD graphics hw encoder <br>- UWP platform Support |

## FAQ

Read [this page](Packages/com.unity.template.renderstreaming/Documentation~/en/faq.md) and [Unity Forum](https://forum.unity.com/forums/unity-render-streaming.413/)

## Contributors

- [@karasusan](https://github.com/karasusan)
- [@hiroki-o](https://github.com/hiroki-o)
- [@flame99999](https://github.com/flame99999)
- [@koseyile](https://github.com/koseyile)
- [@sindharta](https://github.com/sindharta)
- [@kannan-xiao4](https://github.com/kannan-xiao4)
