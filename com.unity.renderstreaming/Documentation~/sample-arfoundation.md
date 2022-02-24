# AR Foundation Sample

This sample demonstrates operating the position and direction of a camera on another PC from the mobile device supported by [AR Foundation](https://docs.unity3d.com/Packages/com.unity.xr.arfoundation@latest).

> [!NOTE]
> If no codec is available on both sender and receiver, the video can not stream. Please note that the compatibility of codecs is different corresponding to platforms and graphics API.

## Additional packages

To use this sample, You need to install following additional packages.

- `ARFoundation 4.1.9`
- `XR Plugin Management 4.2`

> [!NOTE]
> `AR Foundation 4.1.9` is compatible with the following versions of the Unity Editor:
> - 2019.4.15f1 or later
> - 2020.3
> - 2021.1
> - 2021.2

In addition, depending on the platform, each `XR plugin` package must be installed.
​
| Platform | packages |
| -------- | -------- |
| iOS      | `ARKit XR Plugin 4.1.9` |
| Android  | `ARCore XR Plugin 4.1.9` |

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

> [!NOTE]
> If you get an error about Gradle version when building with Unity 2019.4, please refer to [this page](https://developers.google.com/ar/develop/unity/android-11-build).

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