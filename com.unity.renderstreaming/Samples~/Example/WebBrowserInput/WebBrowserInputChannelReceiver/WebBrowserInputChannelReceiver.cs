using System;
using Unity.WebRTC;
using UnityEngine;
using UnityEngine.InputSystem;

namespace Unity.RenderStreaming.Samples
{
    /// <summary>
    ///
    /// </summary>
    [Serializable]
    class ButtonClickEvent : UnityEngine.Events.UnityEvent<int> { }

    /// <summary>
    ///
    /// </summary>
    [Serializable]
    class ButtonClickElement
    {
        [Tooltip("Specifies the ID on the HTML")]
        public int elementId;

        public ButtonClickEvent click;
    }

    /// <summary>
    ///
    /// </summary>
    public class WebBrowserInputChannelReceiver : InputChannelReceiverBase
    {
        /// <summary>
        ///
        /// </summary>
        [SerializeField, Tooltip("Array to set your own click event")]
        private ButtonClickElement[] arrayButtonClickEvent;

        /// <summary>
        ///
        /// </summary>
        public override event Action<InputDevice, InputDeviceChange> onDeviceChange;

        private RemoteInput remoteInput;

        /// <summary>
        ///
        /// </summary>
        /// <param name="track"></param>
        public override void SetChannel(string connectionId, RTCDataChannel channel)
        {
            if (channel == null)
            {
                if (remoteInput != null)
                {
                    onDeviceChange?.Invoke(remoteInput.RemoteGamepad, InputDeviceChange.Removed);
                    onDeviceChange?.Invoke(remoteInput.RemoteKeyboard, InputDeviceChange.Removed);
                    onDeviceChange?.Invoke(remoteInput.RemoteMouse, InputDeviceChange.Removed);
                    onDeviceChange?.Invoke(remoteInput.RemoteTouchscreen, InputDeviceChange.Removed);

                    if (Channel != null)
                    {
                        Channel.OnMessage -= remoteInput.ProcessInput;
                    }

                    remoteInput.Dispose();
                    remoteInput = null;
                }
            }
            else
            {
                remoteInput = RemoteInputReceiver.Create();
                remoteInput.ActionButtonClick = OnButtonClick;
                channel.OnMessage += remoteInput.ProcessInput;
                onDeviceChange?.Invoke(remoteInput.RemoteGamepad, InputDeviceChange.Added);
                onDeviceChange?.Invoke(remoteInput.RemoteKeyboard, InputDeviceChange.Added);
                onDeviceChange?.Invoke(remoteInput.RemoteMouse, InputDeviceChange.Added);
                onDeviceChange?.Invoke(remoteInput.RemoteTouchscreen, InputDeviceChange.Added);
            }
            base.SetChannel(connectionId, channel);
        }

        /// <summary>
        ///
        /// </summary>
        /// <param name="elementId"></param>
        public virtual void OnButtonClick(int elementId)
        {
            foreach (var element in arrayButtonClickEvent)
            {
                if (element.elementId == elementId)
                {
                    element.click.Invoke(elementId);
                }
            }
        }

        /// <summary>
        ///
        /// </summary>
        public virtual void OnDestroy()
        {
            remoteInput?.Dispose();
        }
    }
}
