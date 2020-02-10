---
uid: index.md
title: index
---

# Unity Render Streaming

This package contains public APIs and project samples that are built on top of our [**Unity Render Streaming**](../../com.unity.template.renderstreaming/Documentation~/index.md).

> [!Video https://www.youtube.com/embed/c2pp_T5xzeU]

## Requirements

This version of Render Streaming is compatible with the following versions of the Unity Editor
- **Unity 2019.3**

> [!NOTE]
> Because this template depends on `HDRP 7.1`, it does not work correctly old Unity version.

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

## Other Languages

- [Japanese](./jp/index.md)