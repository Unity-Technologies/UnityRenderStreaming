# AR Foundation Sample

This sample demonstrates operating the position and direction of a camera on another PC from the mobile device supported by [AR Foundation](https://docs.unity3d.com/Packages/com.unity.xr.arfoundation@latest).

> [!NOTE]
> If no codec is available on both sender and receiver, the video can not stream. Please note that the compatibility of codecs is different corresponding to platforms and graphics API.

## Sample scenes

There are two scenes in this sample because the verified package version of AR Foundation is different depends on Unity version.

| Unity version | scene name    |
| ------------- | ------------- |
| Unity2019.4   | `ARFoundation2.1` |
| Unity2020.3   | `ARFoundation4.1` |

## Additional packages

To use this sample, You need to install additional packages shown following table.

| Unity version | Platform | packages |
| ------------- | -------- | -------- |
| Unity2019.4   | Android, iOS | `ARFoundation 2.1`, `XR Plugin Management 4.2` |
| Unity2020.3   | Android, iOS | `ARFoundation 4.1`, `XR Plugin Management 4.2` |

In addition, depending on the platform, each `XR plugin` package must be installed.
​
| Unity version | Platform | packages |
| ------------- | -------- | -------- |
| Unity2019.4   | iOS      | `ARKit XR Plugin 2.1` |
| Unity2019.4   | Android  | `ARCore XR Plugin 2.1` |
| Unity2020.3   | iOS      | `ARKit XR Plugin 4.1` |
| Unity2020.3   | Android  | `ARCore XR Plugin 4.1` |

## Build settings

### iOS

In case of iOS, Please enable to `Requires ARKit support` on `Player Settings`. Additionally, `Architecture` should be selected `ARM64`.

![Player Settings](images/playersettings_arkit.png)

Open **Project Settings** window and select **XR Plug-in Management**. Set enable `ARKit` option in iOS tab.

![XRPlugin Enable ARKit](images/xrplugin_enable_arkit.png)

### Android

In case of Android, **Target Architectures** should be checked `ARM64`, and not checked `ARMv7`.

![Player Settings](images/playersettings_arcore.png)

Open **Project Settings** window and select **XR Plug-in Management**. Set enable `ARCore` option in Android tab.

![XRPlugin Enable ARCore](images/xrplugin_enable_arcore.png)

## Usage

### Hardware acceleration 

Recommend enabling `Hardware Encoder support` on the `Render Streaming` inspector to improve performance using hardware acceleration.

### Procedure

1) Launch the signaling server using public mode. Please refer to [this link](webapp.md) for launching.

![Launch web server](images/launch_webserver_public_mode.png)

2) Build an app in Unity Editor, and launch an application on mobile device.

3) Open **Menu** scene in Unity Editor and click **Broadcast** button.

![Open Broadcast scene](images/open_broadcast_scene.png)

4) On mobile device, configure signaling settings, and click `AR` button.

![Open AR scene](images/open_ar_scene.png)

5) Push the `Start` button on the device and check streaming video.

![Play AR sample](images/play_sample_ar.png)