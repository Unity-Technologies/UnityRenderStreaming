# WebRTC

- [Japanese](../jp/index.md)

WebRTC for Unity is a package that allows [WebRTC](https://webrtc.org) to be used in Unity.

## Guide

* [Tutorial](../en/tutorial.md)

## Installation
To install the package, download WebRTC for Unity from the package manager. See the [documentation](https://docs.unity3d.com/Packages/com.unity.package-manager-ui@latest/index.html) for details on how to use the package manager. 

<img src="./images/webrtc_package_manager.png" width=600 align=center>

## Samples
The package contains the following 3 samples. 

| Scene       | Details                                 |
| -------------- | ------------------------------------ |
| PeerConnection | A scene for checking the process of connecting to a peer |
| DataChannel    | A scene for sending and receiving text       |
| MediaStream    | A scene for sending and receiving video/audio    |

## Limitations

This package uses GPU hardware acceleration for video encoding, so it only runs on graphics cards that support NVCodec.

## Update History

|Date|Reason|
|---|---|
|June 21, 2019|Document Released|
