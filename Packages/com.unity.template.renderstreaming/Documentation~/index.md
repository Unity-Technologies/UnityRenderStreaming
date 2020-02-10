# Unity Render Streaming template

**Unity Render Streaming** is a solution that provides Unity's high definition rendering abilities via browser. It's designed to meet the needs of tasks like viewing car configurators or architectural models on mobile devices.  
This solution's streaming technology takes advantage of [WebRTC](https://webrtc.org/),  and through customization developers can create their own unique solutions.

![Browser HDRP scene](images/browser_hdrpscene.png)

## Requirements

This template is compatible with Unity Editor version below. 
- Unity 2019.3

> [!NOTE]
> Because this template depends on `HDRP 7.1`, it does not work correctly old Unity version.

> [!WARNING]
> In the current version, The combination of Linux and HDRP is not supported. Need to select`OpenGL` in "Graphics API" and use `Legacy` rendering pipeline.

### Hardware encoder

This solution is optimised for NVIDIA graphics cards. Using a recommended graphics card will allow streaming with lower latency. See NVIDIA's [NVIDIA VIDEO CODEC SDK](https://developer.nvidia.com/video-encode-decode-gpu-support-matrix) webpage for a list of recommended graphics cards.

### Browsers

- Desktop Chrome
- Desktop Firefox
- Desktop Safari
- Desktop Microsoft Edge (Chromium-based)
- Android Chrome
- iOS Safari

## Other languages

- [Japanese](jp/index.md)