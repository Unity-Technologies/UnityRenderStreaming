# AR Foundation Samples
This sample shows how to integrate [AR Foundation](https://docs.unity3d.com/Packages/com.unity.xr.arfoundation@latest) with Unity Render Streaming.

## Usage

This sample achieves to control another Unity application from the iOS device remotely.
The scene works as a video receiver and synchronizes the device position and rotation with a camera on the other Unity.

To try this sample, you need another sample scene (`Broadcast.unity`) as a sender-side. 

1. Launch signaling server.
2. Set the IP address of the signaling server to `Render Streaming` component.
3. Build this scene and install to an iOS device.
4. Open a `Broadcast.unity` scene and run.
5. Launch the installed app on iOS device and press a `Start` button.

## Install additional packages

To use AR Foundation on a target device, you need to install additional packages below:

- [ARKit XR Plugin](https://docs.unity3d.com/Packages/com.unity.xr.arkit@4.1/manual/index.html) on iOS

If you need more info to install packages, please see the [documents](https://docs.unity3d.com/Packages/com.unity.xr.arfoundation@latest/manual/index.html) of AR Foudation. 

## Version combination table

Please refer to the following table and select the sample scene.

| Sample Scene Name | Unity Version | ARFoundation Version |
| --------------- | ------------- | -------------------- |
| ARFoundation2.1 |    2019.4     | [2.1 (verified)](https://github.com/Unity-Technologies/arfoundation-samples/tree/2.1)         |
| ARFoundation4.0 |    2020.2     | [4.0 (verified)](https://github.com/Unity-Technologies/arfoundation-samples/tree/4.0)         |

## ARFoundation2.1

This sample is made for **Unity 2019.4 LTS** and [**AR Foundation 2.1**](https://docs.unity3d.com/Packages/com.unity.xr.arfoundation@2.1).

### InputAction with AR Foundation

An `InputBinding` is used for binding input events and callbacks. To use the `InputBinding`, a [Control path](https://docs.unity3d.com/Packages/com.unity.inputsystem@1.0/manual/Controls.html#control-paths) is needed to identify the controls.
AR Foundation provides paths to get device position, rotation, and others.

| Control | Type |
| ----------------- | -------------- |
| deviceposition    | Vector3Control |
| devicerotation    | Vector4Control |
| leftEyePosition   | Vector3Control |
| leftEyeRotation   | Vector4Control |
| rightEyePosition  | Vector3Control |
| rightEyeRotation  | Vector4Control |
| centerEyePosition | Vector3Control |
| centerEyeRotation | Vector4Control |
| trackingState     | IntegerControl |
| isTracked         | ButtonControl  |

The control path contains `<layoutname>` which represents a specific composition of Input Controls. 
You can use `XRInputV1::ARKit` for the `<layoutname>` when using ARKit on iOS.

For example, if you want to get position and rotation of the iOS device, you can set the following paths to `InputAction` on the inspector.

- `<XRInputV1::ARKit>/deviceposition`
- `<XRInputV1::ARKit>/devicerotation`

## ARFoundation4.0

This sample is made for **Unity 2020.2** and [**AR Foundation 4.0**](https://docs.unity3d.com/Packages/com.unity.xr.arfoundation@4.0).
