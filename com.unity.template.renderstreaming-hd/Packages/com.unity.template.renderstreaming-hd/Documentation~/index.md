# Unity Render Streaming HDRP template

**Unity Render Streaming** is a solution that provides Unity's high definition rendering abilities via browser. It's designed to meet the needs of tasks like viewing car configurators or architectural models on mobile devices.  

This solution's streaming technology takes advantage of [WebRTC](https://webrtc.org/),  and through customization developers can create their own unique solutions.

Please take a look at [the tutorial document](en/tutorial.md) if you want to try it right away. Also, confirm [the package document](https://docs.unity3d.com/Packages/com.unity.renderstreaming@latest/index.html) if you want to know about technical detail.

![Browser HDRP scene](images/browser_hdrpscene.png)

## Requirements

This template is compatible with Unity Editor version below. 

- Unity 2019.4

> [!NOTE]
> Because this template depends on `HDRP 7.3`, it does not work correctly older Unity versions.

> [!WARNING]
> In the current version, The combination of Linux and HDRP is not supported. Need to select `OpenGL` in "Graphics API" and use `Legacy` rendering pipeline.

### Hardware encoder

This solution is optimised for NVIDIA graphics cards. Using a recommended graphics card will allow streaming with lower latency. See NVIDIA's [NVIDIA VIDEO CODEC SDK](https://developer.nvidia.com/video-encode-decode-gpu-support-matrix) webpage for a list of recommended graphics cards.

### Browsers

| Browser                           | Windows            | Mac                | iOS                | Android            |
| --------------------------------- | ------------------ | ------------------ | ------------------ | ------------------ |
| Google Chrome                     | :white_check_mark: | :white_check_mark: |                    | :white_check_mark: |
| Safari                            |                    | :white_check_mark: | :white_check_mark: |                    |
| Firefox                           | :white_check_mark: |                    |                    |                    |
| Microsoft edge (Chromium based)   | :white_check_mark: |                    |                    |                    |

## Other languages

- [Japanese](jp/index.md)