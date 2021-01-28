using System;
using Unity.WebRTC;
using UnityEngine;
using UnityEngine.InputSystem;

namespace Unity.RenderStreaming
{
    public class InputSystemChannelReceiver : DataChannelBase, IDataChannel
    {
        [SerializeField] private SimpleCameraController controller;


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
            receiver.onDeviceChange += OnDeviceChange;
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

        private void OnDeviceChange(InputDevice device, InputDeviceChange change)
        {
            switch (change)
            {
                case InputDeviceChange.Added:
                    OnAddDevice(device);
                    return;
            }
        }

        private void OnAddDevice(InputDevice device)
        {
            controller?.SetDevice(device);
        }
    }
}
