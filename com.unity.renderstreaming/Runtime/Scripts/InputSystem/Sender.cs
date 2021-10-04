// todo(kazuki):: This script should be moved into the WebRTC package.
// #if UNITY_WEBRTC_ENABLE_INPUT_SYSTEM
using System;
using System.Collections.Generic;
using System.Linq;
using Unity.WebRTC;
using UnityEngine;
using UnityEngine.InputSystem;
using UnityEngine.InputSystem.LowLevel;
using UnityEngine.InputSystem.Utilities;

// namespace Unity.WebRTC.InputSystem
namespace Unity.RenderStreaming
{
    class Sender : InputManager, IDisposable
    {
        public override event Action<InputEventPtr, InputDevice> onEvent;
        public override event Action<InputDevice, InputDeviceChange> onDeviceChange;
        public override event Action<string, InputControlLayoutChange> onLayoutChange;

        public Sender()
        {
            InputSystem.onEvent += OnEvent;
            InputSystem.onDeviceChange += OnDeviceChange;
            InputSystem.onLayoutChange += OnLayoutChange;
        }

        ~Sender()
        {
            this.Dispose();
        }

        public void Dispose()
        {
            InputSystem.onEvent -= OnEvent;
            InputSystem.onDeviceChange -= OnDeviceChange;
            InputSystem.onLayoutChange -= OnLayoutChange;
        }

        public override ReadOnlyArray<InputDevice> devices
        {
            get
            {
                return InputSystem.devices;
            }
        }

        public override IEnumerable<string> layouts
        {
            get
            {
                // todo(kazuki):: filter layout
                //return InputSystem.ListLayouts();
                return Enumerable.Empty<string>();
            }
        }

        private void OnEvent(InputEventPtr ptr, InputDevice device)
        {
            onEvent?.Invoke(ptr, device);
        }
        private void OnDeviceChange(InputDevice device, InputDeviceChange change)
        {
            onDeviceChange?.Invoke(device, change);
        }
        private void OnLayoutChange(string name, InputControlLayoutChange change)
        {
            onLayoutChange?.Invoke(name, change);
        }
    }

    /// <summary>
    ///
    /// </summary>
    class Observer : IObserver<InputRemoting.Message>
    {
        private RTCDataChannel _channel;
        private bool _isOpen;
        public Observer(RTCDataChannel channel)
        {
            _channel = channel ?? throw new ArgumentNullException("channel is null");
            _channel.OnOpen += () => { _isOpen = true; };
            _channel.OnClose += () => { _isOpen = false; };
            _isOpen = _channel.ReadyState == RTCDataChannelState.Open;
        }
        public void OnNext(InputRemoting.Message value)
        {
            if (!_isOpen)
                return;
            byte[] bytes = MessageSerializer.Serialize(ref value);
            _channel.Send(bytes);
        }

        public void OnCompleted()
        {
        }
        public void OnError(Exception error)
        {
        }
    }
}
// #endif
