using System;
using Unity.WebRTC;
using UnityEngine.InputSystem;

namespace Unity.RenderStreaming
{
    public class WebBrowserInputChannelReceiver : DataChannelBase
    {
        /// <summary>
        /// 
        /// </summary>
        public event Action<InputDevice, InputDeviceChange> onDeviceChange;

        private RemoteInput remoteInput;

        /// <summary>
        /// 
        /// </summary>
        /// <param name="track"></param>
        public override void SetChannel(string connectionId, RTCDataChannel channel)
        {
            if (channel == null)
            {
                remoteInput?.Dispose();
                return;
            }
            remoteInput = RemoteInputReceiver.Create();
            onDeviceChange.Invoke(remoteInput.RemoteGamepad, InputDeviceChange.Added);
            onDeviceChange.Invoke(remoteInput.RemoteKeyboard, InputDeviceChange.Added);
            onDeviceChange.Invoke(remoteInput.RemoteMouse, InputDeviceChange.Added);
            onDeviceChange.Invoke(remoteInput.RemoteTouchscreen, InputDeviceChange.Added);

            base.SetChannel(connectionId, channel);
        }

        public void OnDestroy()
        {
            remoteInput?.Dispose();
        }
    }
}
