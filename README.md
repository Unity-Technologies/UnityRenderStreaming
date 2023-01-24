# Unity Render Streaming

<img src="https://img.shields.io/badge/unity-2020.3-green.svg?style=flat-square" alt="unity 2020.3">
<img src="https://img.shields.io/badge/unity-2021.3-green.svg?style=flat-square" alt="unity 2021.3">
<img src="https://img.shields.io/badge/unity-2022.1-green.svg?style=flat-square" alt="unity 2022.1">

**Unity Render Streaming** is a solution that provides Unity's high quality rendering abilities via browser. It's designed to meet the needs of tasks like viewing car configurators or architectural models on mobile devices.
This solution's streaming technology takes advantage of [WebRTC](https://webrtc.org/), and developers can even use the [WebRTC package](https://docs.unity3d.com/Packages/com.unity.webrtc@latest) to create their own unique solutions.

<img src="com.unity.renderstreaming/Documentation~/images/browser_hdrpscene.png" width=500 align=center>

## Requirements

Please see [Requirements](com.unity.renderstreaming/Documentation~/index.md#requirements) section.

### Furioos compatibility

**Unity Render Streaming** is also supported natively by **Furioos** platform https://www.furioos.com/ .
That means that you can easily build a Unity application, upload it on **Furioos** and enjoy all the features of **Unity Render Streaming** without worrying about the deployment and scalability issues of your project.

Please see [Furioos Tutorial](com.unity.renderstreaming/Documentation~/deploy-to-furioos) section to find out how it works.

### License

- `com.unity.renderstreaming` -  [LICENSE.md](com.unity.renderstreaming/LICENSE.md)

- `com.unity.template.renderstreaming-hd` -  [LICENSE.md](com.unity.template.renderstreaming-hd/Packages/com.unity.template.renderstreaming-hd/LICENSE.md)

- `com.unity.template.renderstreaming-rtx` -  [LICENSE.md](com.unity.template.renderstreaming-rtx/Packages/com.unity.template.renderstreaming-rtx/LICENSE.md)

## Repository Structure

```
<root>
├── BuildScripts~                          // Build scripts for CI system
├── com.unity.renderstreaming              // Unity Render Streaming Package
├── com.unity.template.renderstreaming-hd  // HDRP template
├── com.unity.template.renderstreaming-rtx // HDRP Raytracing template
├── RenderStreaming~                       // Sample project for package
└── WebApp                                 // Web application for signaling
```

## Roadmap

| Version | Focus | When |
| ------- | ----- | ----- |
| `1.0.0-preview` | - First release | Aug 2019 |
| `1.1.0-preview` | - Upgrade HDRP version 5.16 | Sep 2019 |
| `1.2.0-preview` | - *Unity 2019.3* support | Feb 2020 |
| `2.0.0-preview` | - Multi camera <br/>- DirectX12 (DXR) Support | Apr 2020 |
| `2.1.0-preview` | - *Unity 2019.4* support <br/>- Add bitrate control sample | Aug 2020 |
| `2.2.0-preview` | - Add video receiver <br/>- HDRP/URP on Linux support | Nov 2020 |
| `3.0.0-preview` | - iOS platform support <br/>- AR Foundation sample | Mar 2021 |
| `3.1.0-exp.1` | - Android platform support | Jun 2021 |
| `3.1.0-exp.2` | - Audio Renderer support <br/> - Multiplay sample <br/> - M1 Mac support | Dec 2021 |
| `3.1.0-exp.3` | - Fix bugs | Feb 2022 |
| `3.1.0-exp.4` | - Streaming settings API <br/> - *Unity 2022.1* support <br/> - Remove *Unity 2019.4* from support list | Oct 2022 |
| `3.1.0-exp.5` | - Fix bugs | Jan 2023 |
| `3.1.0-exp.6` | - Streaming Settings Window <br/> - Auto Configuration <br/> - Command line option | Feb 2023 |

## FAQ

Please read [this page](com.unity.renderstreaming/Documentation~/faq.md) and [Unity Forum](https://forum.unity.com/forums/unity-render-streaming.413/).

## Contributors

- [@karasusan](https://github.com/karasusan)
- [@hiroki-o](https://github.com/hiroki-o)
- [@flame99999](https://github.com/flame99999)
- [@koseyile](https://github.com/koseyile)
- [@sindharta](https://github.com/sindharta)
- [@kannan-xiao4](https://github.com/kannan-xiao4)
- [@samuel-tranchet](https://github.com/samuel-tranchet)
