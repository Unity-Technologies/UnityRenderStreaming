# Creating The Scene

This tutorial explains how to edit an empty scene in Unity Editor and display the image rendered from the camera to the browser.

## Setting component

If Unity version is 2020 or later, Create a new empty scene. Select **File > New Scene** from the menu to open the dialog and select **Basic (Built-in)**.

![Create new scene](images/create_new_scene.png)

Select the **Main Camera** object in the Hierarchy window, and add the **Render Streaming** component in the Inspector window.

![Add RenderStreaming component](images/add_renderstreaming_component.png)

Update properties in the inspector.

- **Signaling Type** : `WebSocketSignaling`
- **Signaling URL** : `ws://localhost`

![Create scene 03](images/change_properties_websocket.png)

Next, we will add the **Broadcast** component to the **Main Camera** object. this component deliver the stream to multiple peers.

![Add Broadcast component](images/add_broadcast_component.png)

Assign the **Broadcast** component to the `Handler` property of the **Render Streaming** component.

![Assign Broadcast component to handler](images/assign_broadcast_to_handler.png)

Add a [**Video Stream Sender**](video-streaming.html#videostreamsenderapiunityrenderstreamingvideostreamsenderhtml-component) component to the **Main Camera** object. This component refers to the Camera and delivers it as a stream to other peers.

![Add VideoStreamSender component](images/add_videostreamsender_component.png)

Assign the [**Video Stream Sender**](video-streaming.html#videostreamsenderapiunityrenderstreamingvideostreamsenderhtml-component) component to the **Broadcast** component property. And set camera

![Assign VideoStreamSender component to streams](images/assign_videostreamsender_to_streams.png)

### Check on browser

Open the **Receiver Sample** page, and you should see the video from the Unity camera appear in your browser.

![Receiver Sample on the browser](images/receiver_sample_on_browser.png)

## Next Step

You can distribute video from Unity to the browser. It's also possible to stream audio. There are several types of components that can be streamed. Check the [component page](components.md) for details.

Next, please try to control the camera from the browser. Please go to [the next page](control-camera.md).
