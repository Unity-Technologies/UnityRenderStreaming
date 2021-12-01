# Samples

The package contains the following samples. 

| Scene | Details |
| ----- | ------- |
| [Receiver](sample-receiver.md) | Displaying a video streaming from another Unity |
| [Broadcast](sample-broadcast.md) | Streaming video and audio to multi Unity applications |
| [Bidirectional](sample-bidirectional.md) | bi-directional(send/receive) each other's video |
| [Multiplay](sample-multiplay.md) | Multi player game using video streaming |
| [RenderPipeline](sample-renderpipeline.md) | Streaming video using HDRP or URP |
| [AR Foundation](sample-arfoundation.md) | Controling a camera using AR Foundation |
| [Gyroscope](sample-gyroscope.md) | Controling a camera using Gyroscope |
| [Web Browser Input](sample-browserinput.md) | Streaming the video from Unity to the web browser |


## Import samples in project
You can import samples from the bottom of the `com.unity.renderstreaming` package in the **Package Manager** Window.

![Sample List](images/renderstreaming_samples.png)

## Build Settings

It is easy to try all samples if only you put sample scenes into `Scenes in Build` in **Build Settings** Window. **Menu** scene should be loaded as a first one so you need to rearrange scenes.

![Build Settings](images/sample_buildsettings.png)

## Sample menu

**Menu** scene is helpful for checking multiple sample scenes. You can find the scene `Example/Menu/Menu.scene`.

![Sample menu](images/sample_menu.png)

Open **Menu** scene and press `Play` button, as you can see the menu for sample scenes.

![Sample menu](images/sample_menu_scene.png)

Users can change signaling settings below during runtime.

- [**Signaling Type**](signaling-type.md)
- **Enable/Disable SSL**
- **Host Address**
- **Interval (only Http signaling)**

![Sample menu](images/sample_menu_setting_panel.png)