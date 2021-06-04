using System;
using Unity.WebRTC;
using UnityEngine.InputSystem;

namespace Unity.RenderStreaming
{
    /// <summary>
    /// 
    /// </summary>
    public class InputSystemChannelReceiver : InputChannelReceiverBase
    {
        /// <summary>
        ///
        /// </summary>
        public override event Action<InputDevice, InputDeviceChange> onDeviceChange;

        private Receiver receiver;
        private InputRemoting receiverInput;
        private IDisposable subscriberDisposer;

        /// <summary>
        ///
        /// </summary>
        /// <param name="track"></param>
        public override void SetChannel(string connectionId, RTCDataChannel channel)
        {
            if (channel == null)
            {
                Dispose();
            }
            else
            {
                receiver = new Receiver(channel);
                receiver.onDeviceChange += onDeviceChange;
                receiverInput = new InputRemoting(receiver);
                subscriberDisposer = receiverInput.Subscribe(receiverInput);
                receiverInput.StartSending();
            }
            base.SetChannel(connectionId, channel);
        }

        /// <summary>
        /// 
        /// </summary>
        protected virtual void OnDestroy()
        {
            Dispose();
        }

        protected void Dispose()
        {
            receiverInput?.StopSending();
            subscriberDisposer?.Dispose();
            receiver?.Dispose();
            receiver = null;
        }
    }
}
