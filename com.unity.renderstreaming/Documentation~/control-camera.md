# Setting Up Device Input

To control the camera on the remote PC,  we need to link the input events on web browsers to the actions in Unity. **Input System** makes it easy to implement the input processing.

## Check settings of Input System

First, please check the [Wizard window](wizard.md) about the Input System settings.

## Adding components for input processing

Open the Unity scene which created in [the previous page](create-camera.md), and select **Main Camera** in the Hierarchy window. Add the **Input Receiver** component in the Inspector window.

![Add InputReceiver component](images/add_inputreceiver_component.png)

The initial condition of the inspector of **Input Receiver** is below.

![Add InputReceiver component](images/inputreceiver_inspector.png)

Add the **Input Receiver** component to the **Broadcast** component property.

![Assign InputReceiver to streams](images/assign_inputreceiver_to_streams.png)

## Setting Input Actions

We will use the [**Input Actions**](https://docs.unity3d.com/Packages/com.unity.inputsystem@1.0/manual/Actions.html) feature of the Input System. Input Actions is a mechanism for mapping various inputs to Unity actions, and it provides editing tools that make it easy for anyone to use. We will use Input Actions to define mouse actions.

Right-click in the project window, and then click **Create > Input Actions**.

![Control camera 01](images/control_camera_01.png)

The asset will be added to the project window and rename to **Control**. Double-click on the **Control** asset in the project window, and the **Action editor** will appear.

![Control camera 01](images/control_camera_02.png)

Click the **+** button in **Actions Maps** to add an item.

![Control camera 01](images/control_camera_03.png)

Rename the **Actions** item to `Look`.

![Control camera 01](images/control_camera_04.png)

Select the item and change **Action Type** to `Value`, and **Control Type** to `Vector2`.

![Control camera 01](images/control_camera_05.png)

Next, select the **\<No Binding\>** item and add **Mouse > Delta** to the **Path** of **Binding**.

![Control camera 01](images/control_camera_06.png)

The result will look like this. Note that the last step is to press the **Save Asset** button to save your edits.

![Control camera 01](images/control_camera_07.png)

## Setting [`InputReceiver`](../api/Unity.RenderStreaming.InputReceiver.html) component

Drag the **Control** asset to the **Input Actions** property of the [`InputReceiver`](../api/Unity.RenderStreaming.InputReceiver.html) component.

![Control camera 01](images/assign_inputactions_to_inputreceiver.png)

Add the code to control the rotation of the camera. Move to the Inspector window, click **Add component** button and select **New Script** Script to create the asset. Change the asset name to `PlayerControler`.

![Create Player Controller component](images/create_playercontroler_component.png)

The contents of the script will be as follows.

```csharp
using UnityEngine;
using UnityEngine.InputSystem;

public class PlayerController : MonoBehaviour
{
    public void Look(InputAction.CallbackContext value)
    {
        Vector2 input = value.ReadValue<Vector2>();
        transform.eulerAngles += new Vector3(-input.y, input.x, 0);
    }
}
```

Once you have copied the script and saved the file, assign `PlayerController.Look` to the [`InputReceiver`](../api/Unity.RenderStreaming.InputReceiver.html) component's **Look** event.

![Control camera 01](images/assign_event_to_inputreceiver.png)

## Checking on browsers

The mouse operation in the browser should be reflected in the rotation of the camera. If the camera is not moved, please check the settings on the [Wizard](wizard.md) window.

## Next step

We were able to incorporate the input processing using the Input Actions feature. If you would like to learn more about the features of Unity Render Streaming, please see the [Sample page](samples.md). Also, check out [Customizing your web application](customize-webapp.md) for information on how to modify the web user interface.
