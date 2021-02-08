using System;
using Unity.WebRTC;
using UnityEngine.InputSystem;

namespace Unity.RenderStreaming
{
    public class InputSystemChannelReceiver : DataChannelBase
    {
        /// <summary>
        /// 
        /// </summary>
        public event Action<InputDevice, InputDeviceChange> onDeviceChange;

        private Receiver receiver;
        private InputRemoting receiverInput;
        private IDisposable receiverDisposer;

        /// <summary>
        /// 
        /// </summary>
        /// <param name="track"></param>
        public override void SetChannel(string connectionId, RTCDataChannel channel)
        {
            receiver = new Receiver(channel);
            receiver.onDeviceChange += onDeviceChange;
            receiverInput = new InputRemoting(receiver);
            receiverDisposer = receiverInput.Subscribe(receiverInput);
            receiverInput.StartSending();

            base.SetChannel(connectionId, channel);
        }

        public void OnDestroy()
        {
            receiverInput?.StopSending();
            receiverDisposer?.Dispose();
        }
    }
}
