// todo(kazuki):: This script should be moved into the WebRTC package.
// #if UNITY_WEBRTC_ENABLE_INPUT_SYSTEM
using System;
using Unity.WebRTC;
using UnityEngine;
using UnityEngine.InputSystem;
using UnityEngine.InputSystem.Layouts;
using UnityEngine.InputSystem.LowLevel;
using UnityEngine.InputSystem.Utilities;

// namespace Unity.WebRTC.InputSystem
namespace Unity.RenderStreaming
{
    /// <summary>
    /// 
    /// </summary>
    public interface IInputManager
    {
        /// <summary>
        /// 
        /// </summary>
        event Action<InputRemoting.Message> onMessage;
        /// <summary>
        /// 
        /// </summary>
        event Action<InputEventPtr, InputDevice> onEvent;
        /// <summary>
        /// 
        /// </summary>
        event Action<InputDevice, InputDeviceChange> onDeviceChange;
        /// <summary>
        /// 
        /// </summary>
        event Action<string, InputControlLayoutChange> onLayoutChange;

        /// <summary>
        /// 
        /// </summary>
        ReadOnlyArray<InputDevice> devices { get; }
        /// <summary>
        /// 
        /// </summary>
        /// <param name="deviceId"></param>
        /// <returns></returns>
        InputDevice GetDeviceById(int deviceId);
        /// <summary>
        /// 
        /// </summary>
        /// <param name="layout"></param>
        /// <param name="name"></param>
        /// <param name="variants"></param>
        /// <returns></returns>
        InputDevice AddDevice(string layout, string name = null, string variants = null);
        /// <summary>
        /// 
        /// </summary>
        /// <param name="device"></param>
        void RemoveDevice(InputDevice device);
        /// <summary>
        /// 
        /// </summary>
        /// <param name="device"></param>
        /// <param name="usage"></param>
        void SetDeviceUsage(InputDevice device, string usage);
        /// <summary>
        /// 
        /// </summary>
        /// <param name="name"></param>
        /// <returns></returns>
        InputControlLayout LoadLayout(string name);
        /// <summary>
        /// 
        /// </summary>
        /// <param name="json"></param>
        /// <param name="name"></param>
        /// <param name="matches"></param>
        void RegisterLayout(string json, string name = null, InputDeviceMatcher? matches = null);
        /// <summary>
        /// 
        /// </summary>
        /// <param name="name"></param>
        void RemoveLayout(string name);
        /// <summary>
        /// 
        /// </summary>
        /// <param name="eventPtr"></param>
        void QueueEvent(InputEventPtr eventPtr);
    }

    public abstract class InputManager : IInputManager
    {
        //todo(kazuki):: remove warning CS0067
#pragma warning disable 0067
        public virtual event Action<InputRemoting.Message> onMessage;
        public virtual event Action<InputEventPtr, InputDevice> onEvent;
        public virtual event Action<InputDevice, InputDeviceChange> onDeviceChange;
        public virtual event Action<string, InputControlLayoutChange> onLayoutChange;
#pragma warning restore 0067

        public ReadOnlyArray<InputDevice> devices
        {
            get
            {
                return InputSystem.devices;
            }
        }

        public virtual InputDevice GetDeviceById(int deviceId)
        {
            return InputSystem.GetDeviceById(deviceId);
        }

        public virtual InputDevice AddDevice(string layout, string name = null, string variants = null)
        {
            return InputSystem.AddDevice(layout, name, variants);
        }

        public virtual void RemoveDevice(InputDevice device)
        {
            InputSystem.RemoveDevice(device);
        }

        public virtual void SetDeviceUsage(InputDevice device, string usage)
        {
            InputSystem.SetDeviceUsage(device, usage);
        }

        public virtual InputControlLayout LoadLayout(string name)
        {
            return InputSystem.LoadLayout(name);
        }

        public virtual void RegisterLayout(string json, string name = null, InputDeviceMatcher? matches = null)
        {
            InputSystem.RegisterLayout(json, name, matches);
        }

        public virtual void RemoveLayout(string name)
        {
            InputSystem.RemoveLayout(name);
        }

        public virtual void QueueEvent(InputEventPtr eventPtr)
        {
            InputSystem.QueueEvent(eventPtr);
        }
    }

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

    class Sender : InputManager
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
            InputSystem.onEvent -= OnEvent;
            InputSystem.onDeviceChange -= OnDeviceChange;
            InputSystem.onLayoutChange -= OnLayoutChange;
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
            byte[] bytes = value.Serialize();
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
