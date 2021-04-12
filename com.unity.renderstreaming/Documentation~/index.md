# Unity Render Streaming

This package contains public APIs and project samples that are built on top of our **Unity Render Streaming**. We can develop a peer-to-peer streaming solution quickly by using this package.

> [!Video https://www.youtube.com/embed/c2pp_T5xzeU]

## Main features

This package provides the main features below.

- Video streaming
- Audio streaming
- Remote control

### Video streaming

We can broadcast video rendered on Unity to browsers via network. For example, It can show the video rendered with HDRP on the browser on the iPad. In addition, broadcasting from multiple cameras is supported. 

![feature-multicamera](images/feature_multicamera.png)

### Audio streaming
This package enables streaming sounds generated on Unity. And it can cast to multi-browsers simultaneously.

### Remote control

It is possible to send input messages to Unity from the browser, and sending inputs from multiple browsers is supported. Mouse, keyboard, touch, and gamepad are supported as input devices on the browser. For more information, please see the [Browser input process](browser_input.md) page.

## Requirements

This version of Render Streaming is compatible with the following versions of the Unity Editor

- **Unity 2019.4**

### Web Browsers support

Unity Render Streaming supports almost all browsers that can use WebRTC. See [Web browser input sample](sample-browserinput.md).

| Browser                           | Windows            | Mac                | iOS                | Android            |
| --------------------------------- | ------------------ | ------------------ | ------------------ | ------------------ |
| Google Chrome                     | :white_check_mark: | :white_check_mark: |                    | :white_check_mark: |
| Safari                            |                    | :white_check_mark: | :white_check_mark: |                    |
| Firefox                           | :white_check_mark: |                    |                    |                    |
| Microsoft edge (Chromium based)   | :white_check_mark: |                    |                    |                    |

> [!NOTE]
> It may not work properly on some browsers caused by depending on the status of support about WebRTC.

> [!NOTE]
> In **Safari** and **iOS Safari**, WebRTC features cannot be used with **http**. Instead, **https** must be used.

## Samples

Please check [this page](samples.md).

## Project template

Project templates are on the [GitHub repository](https://github.com/Unity-Technologies/UnityRenderStreaming).

## Furioos compatibility

**Unity Render Streaming** is also supported natively by **Furioos** platform https://www.furioos.com/ .
That means that you can easily build a Unity application, upload it on **Furioos** and enjoy all the features of **Unity Render Streaming** without worrying about the deployment and scalability issues of your project.

Please see [Furioos Tutorial](tutorial.md#deploy-to-furioos) section to find out how it works.