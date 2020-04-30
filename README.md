# Unity Render Streaming

<img src="https://img.shields.io/badge/unity-2019.3-green.svg?style=flat-square" alt="unity 2019.3">

**Unity Render Streaming** is a solution that provides Unity's high quality rendering abilities via browser. It's designed to meet the needs of tasks like viewing car configurators or architectural models on mobile devices.  
This solution's streaming technology takes advantage of [WebRTC](https://webrtc.org/), and developers can even use the [WebRTC package](https://github.com/Unity-Technologies/com.unity.webrtc) to create their own unique solutions. 

<img src="com.unity.renderstreaming/Documentation~/images/browser_hdrpscene.png" width=500 align=center>

This repository manages 1 package and 2 templates. Details can be found in the `README.md` for each package.

- [com.unity.template.renderstreaming-hd](com.unity.template.renderstreaming-hd/Packages/com.unity.template.renderstreaming-hd/README.md)
- [com.unity.template.renderstreaming-rtx](com.unity.template.renderstreaming-hd/Packages/com.unity.template.renderstreaming-rtx/README.md)
- [com.unity.renderstreaming](com.unity.renderstreaming/README.md)

## Branch

| Branch                                                                                         | Status         |
| ---------------------------------------------------------------------------------------------- | -------------- |
| [release/2.0.0](https://github.com/Unity-Technologies/UnityRenderStreaming/tree/release/2.0.0) | Stable         |
| [develop](https://github.com/Unity-Technologies/UnityRenderStreaming/tree/develop)             | In development |

## Operating Environment

This solution is compatible with `Unity 2019.3`.

| Platform    | Graphics API | Hardware Encoder                                  | Software Encoder   |
| ----------- | ------------ | ------------------------------------------------- | ------------------ |
| Windows x64 | DirectX11    | :white_check_mark: (Require NVIDIA Graphics card) | :white_check_mark: | 
| Windows x64 | DirectX12    |                                                   |                    | 
| Windows x64 | OpenGL       |                                                   |                    |
| Windows x64 | Vulkan       |                                                   |                    | 
| Linux x64   | OpenGL       | :white_check_mark: (Require NVIDIA Graphics card) |                    |
| Linux x64   | Vulkan       |                                                   |                    |
| MacOS       | OpenGL       |                                                   |                    |
| MacOS       | Metal        |                                                   | :white_check_mark: |

### Supported Browsers

Browser support depends on the status of each browser's WebRTC support.

- Desktop Chrome
- Desktop Firefox
- Desktop Safari
- Desktop Microsoft Edge (Chrominium-based)
- Android Chrome
- iOS Safari

### License

- `com.unity.renderstreaming` -  [LICENSE.md](com.unity.renderstreaming/LICENSE.md)

- `com.unity.template.renderstreaming-hd` -  [LICENSE.md](com.unity.template.renderstreaming-hd/Packages/com.unity.template.renderstreaming-hd/LICENSE.md)

- `com.unity.template.renderstreaming-rtx` -  [LICENSE.md](com.unity.template.renderstreaming-hd/Packages/com.unity.template.renderstreaming-rtx/LICENSE.md)

## Package Structure

```
<root>
├── com.unity.renderstreaming
│   ├── Documentation~
│   ├── Editor
│   └── Samples~
├── com.unity.template.renderstreaming-hd
│   ├── Assets
│   │   ├── Plugins
│   │   ├── Scenes
│   │   ├── Scripts
│   │   └── Tests
│   ├── Packages
│   └── ProjectSettings
├── com.unity.template.renderstreaming-rtx
│   ├── Assets
│   │   ├── Plugins
│   │   ├── Scenes
│   │   ├── Scripts
│   │   └── Tests
│   ├── Packages
│   └── ProjectSettings
├── EmptyProjectForTest~
├── RenderStreamingSampleSrc~
|   ├── RS_HDRPSampleSrc_1.x
│   └── RS_URPSampleSrc_1.x
└── WebApp
    ├── public
    ├── src
    └── test
```

## Roadmap

| Version | Focus                                               |
| ------- | --------------------------------------------------- |
| `1.0`   | - First release                                     |
| `1.1`   | - Upgrade HDRP version 5.16                         |
| `1.2`   | - Unity 2019.3 support                              |
| `2.0`   | - Multi camera <br>- DirectX12 (DXR) Support        |
| `2.1`   | - Unity 2020.1 support <br>- Bitrate control sample |

## FAQ

Read [this page](com.unity.renderstreaming/Documentation~/en/faq.md) and [Unity Forum](https://forum.unity.com/forums/unity-render-streaming.413/)

## Contributors

- [@karasusan](https://github.com/karasusan)
- [@hiroki-o](https://github.com/hiroki-o)
- [@flame99999](https://github.com/flame99999)
- [@koseyile](https://github.com/koseyile)
- [@sindharta](https://github.com/sindharta)
- [@kannan-xiao4](https://github.com/kannan-xiao4)
