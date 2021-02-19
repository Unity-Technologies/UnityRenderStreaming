// todo(kazuki):: This script should be moved into the WebRTC package.
// #if UNITY_WEBRTC_ENABLE_INPUT_SYSTEM
using System;
using System.Collections.Generic;
using System.Linq;
using Unity.WebRTC;
using UnityEngine.InputSystem;
using UnityEngine.InputSystem.Layouts;
using UnityEngine.InputSystem.Utilities;

// namespace Unity.WebRTC.InputSystem
namespace Unity.RenderStreaming
{
    /// <summary>
    ///
    /// </summary>
    class Receiver : InputManager
    {
        public override event Action<InputRemoting.Message> onMessage;
        public new event Action<InputDevice, InputDeviceChange> onDeviceChange;
        public new event Action<string, InputControlLayoutChange> onLayoutChange;

        private RTCDataChannel _channel;
        private readonly List<InputDevice> _remoteDevices = new List<InputDevice>();

        /// <summary>
        ///
        /// </summary>
        /// <param name="channel"></param>
        public Receiver(RTCDataChannel channel)
        {
            _channel = channel ?? throw new ArgumentNullException("channel is null");
            _channel.OnMessage += OnMessage;
        }

        ~Receiver()
        {
            RemoveAllDevices();
        }

        private void OnMessage(byte[] bytes)
        {
            MessageSerializer.Deserialize(bytes, out var message);
            onMessage?.Invoke(message);
        }

        /// <summary>
        ///
        /// </summary>
        public override ReadOnlyArray<InputDevice> devices
        {
            get
            {
                // note:: InputRemoting class rejects remote devices when sending device information to the remote peer.
                // Avoid to get assert "Device being sent to remotes should be a local device, not a remote one"
                return new ReadOnlyArray<InputDevice>();
            }
        }

        /// <summary>
        ///
        /// </summary>
        public override IEnumerable<string> layouts
        {
            get
            {
                return Enumerable.Empty<string>();
            }
        }


        /// <summary>
        ///
        /// </summary>
        public ReadOnlyArray<InputDevice> remoteDevices
        {
            get
            {
                return new ReadOnlyArray<InputDevice>(_remoteDevices.ToArray());
            }
        }

        /// <summary>
        ///
        /// </summary>
        public void RemoveAllDevices()
        {
            while (_remoteDevices.Count > 0)
            {
                RemoveDevice(_remoteDevices[0]);
            }
        }

        public override InputDevice AddDevice(string layout, string name = null, string variants = null)
        {
            var device = base.AddDevice(layout, name, variants);
            _remoteDevices.Add(device);
            onDeviceChange?.Invoke(device, InputDeviceChange.Added);
            return device;
        }

        public override void RemoveDevice(InputDevice device)
        {
            base.RemoveDevice(device);
            _remoteDevices.Remove(device);
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
