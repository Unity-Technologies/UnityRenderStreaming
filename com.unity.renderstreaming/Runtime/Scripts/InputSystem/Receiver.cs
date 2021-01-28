// todo(kazuki):: This script should be moved into the WebRTC package.
// #if UNITY_WEBRTC_ENABLE_INPUT_SYSTEM
using System;
using Unity.WebRTC;
using UnityEngine.InputSystem;
using UnityEngine.InputSystem.Layouts;

// namespace Unity.WebRTC.InputSystem
namespace Unity.RenderStreaming
{
    class Receiver : InputManager
    {
        public override event Action<InputRemoting.Message> onMessage;
        public new event Action<InputDevice, InputDeviceChange> onDeviceChange;
        public new event Action<string, InputControlLayoutChange> onLayoutChange;

        private RTCDataChannel _channel;

        public Receiver(RTCDataChannel channel)
        {
            _channel = channel ?? throw new ArgumentNullException("channel is null");
            _channel.OnMessage += OnMessage;
        }

        private void OnMessage(byte[] bytes)
        {
            InputRemoting.Message.Deserialize(bytes, out var message);
            onMessage?.Invoke(message);
        }

        public override InputDevice AddDevice(string layout, string name = null, string variants = null)
        {
            var device = base.AddDevice(layout, name, variants);
            onDeviceChange?.Invoke(device, InputDeviceChange.Added);
            return device;
        }

        public override void RemoveDevice(InputDevice device)
        {
            base.RemoveDevice(device);
            onDeviceChange?.Invoke(device, InputDeviceChange.Removed);
        }

        public override void RegisterLayout(string json, string name = null, InputDeviceMatcher? matches = null)
        {
            base.RegisterLayout(json, name, matches);
            onLayoutChange?.Invoke(name, InputControlLayoutChange.Added);
        }

        public override void RemoveLayout(string name)
        {
            base.RemoveLayout(name);
            onLayoutChange?.Invoke(name, InputControlLayoutChange.Removed);
        }
    }
}
// #endif
