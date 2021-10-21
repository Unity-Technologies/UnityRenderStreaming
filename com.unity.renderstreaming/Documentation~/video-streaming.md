# Video Streaming

The base class for sending video is `VideoSender`, and the base class for receiving video is `VideoReceiver`. And various components are implemented using the inheritance relation. Components are provided for the sender and receiver sides. There are multiple components for the sender side, but all the sent videos can be received by `TextureReceiver`.

> [!NOTE]
> The choice of video codec is currently limited. Use **H.264** if using a hardware encoder, or **VP8** if using a software encoder.

## `TextureSender` component

This component sends the texture.

![TextureSender inspector](images/texturesender_inspector.png)

### Properties

| Parameter | Description | Default |
| --------- | ----------- | ------- |
| **Streaming Size** | Size of the frame buffer used for streaming | 1280, 720 |
| **Anti-aliasing** | The antialiasing level for the RenderTexture | None |
| **Depth Buffer** | The precision of the render texture's depth buffer in bits | No depth buffer |


## `ScreenSender` component

This component sends the image of the main display.

![ScreenSender inspector](images/displaysender_inspector.png)

### Properties

| Parameter | Description | Default |
| --------- | ----------- | ------- |
| **Streaming Size** | Size of the frame buffer used for streaming | 1280, 720 |
| **Anti-aliasing** | The antialiasing level for the RenderTexture | None |
| **Depth Buffer** | The precision of the render texture's depth buffer in bits | No depth buffer |

## `CameraSender` component

This component streams the `Camera` component's camera rendering results.  Uses `Target Texture` to store the rendering results.

> [!NOTE]
> You can attach the `Target Texture` to the `Camera` component.
> If `Target Texture` is attached on Camera, use that `Render Texture` setting first.

![CameraSender inspector](images/camerasender_inspector.png)

### Properties

| Parameter | Description | Default |
| --------- | ----------- | ------- |
| **Streaming Size** | Size of the frame buffer used for streaming | 1280, 720 |
| **Anti-aliasing** | The antialiasing level for the RenderTexture | None |
| **Depth Buffer** | The precision of the render texture's depth buffer in bits | No depth buffer |

## `WebCamSender` component

This component streams the `WebCamTexture` rendering results. Please refer [WebCamTexture](https://docs.unity3d.com/ScriptReference/WebCamTexture.html) pages.

![WebCamSender inspector](images/webcamsender_inspector.png)

> [!NOTE]
> When building application for iOS platform, you need set `Camera Usage Description` on Player Settings.
> If not set this, your application exits. (refer [this page](https://developer.apple.com/library/archive/documentation/General/Reference/InfoPlistKeyReference/Articles/CocoaKeys.html#//apple_ref/doc/uid/TP40009251-SW24))

![Set Camera Usage Description](images/sample_bidirectional_camerausagedescription.png)

### Properties

| Parameter | Description | Default |
| --------- | ----------- | ------- |
| **Streaming Size** | Size of the frame buffer used for streaming | 1280, 720 |
| **Device Index** | Index of `WebCamTexutre.devices` to use | 0 |


## `TextureReceiver` component

This component receives a videostream and exposes a texture that rendered the receiving frame buffer. 

![TextureReceiver inspector](images/texturereceiver_inspector.png)

### Properties

| Parameter | Description | Default |
| --------- | ----------- | ------- |
| **Streaming Size** | Size of the frame buffer used for streaming | 1280, 720 |
| **Connection Id** | ID of receiving videostream | Empty |