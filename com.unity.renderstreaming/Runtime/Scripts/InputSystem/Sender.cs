using System;
using System.Collections.Generic;
using Unity.WebRTC;
using UnityEngine;
using UnityEngine.InputSystem;
using UnityEngine.InputSystem.LowLevel;
using UnityEngine.InputSystem.Utilities;

namespace Unity.RenderStreaming.InputSystem
{
    using InputSystem = UnityEngine.InputSystem.InputSystem;

    class Sender : InputManager, IDisposable
    {
        public override event Action<InputEventPtr, InputDevice> onEvent;
        public override event Action<InputDevice, InputDeviceChange> onDeviceChange;
        public override event Action<string, InputControlLayoutChange> onLayoutChange;

        private InputPositionCorrector _corrector;
        private Action<InputEventPtr, InputDevice> _onEvent;

        public Sender()
        {
            InputSystem.onEvent += OnEvent;
            InputSystem.onDeviceChange += OnDeviceChange;
            InputSystem.onLayoutChange += OnLayoutChange;

            _onEvent = (InputEventPtr ptr, InputDevice device) => { onEvent?.Invoke(ptr, device); };
            _corrector = new InputPositionCorrector(_onEvent);
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

        /// <summary>
        /// 
        /// </summary>
        public bool EnableInputPositionCorrection { set; get; }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="inputRegion"></param>
        /// <param name="outputRegion"></param>
        public void CalculateInputRegion(Rect inputRegion, Rect outputRegion)
        {
            _corrector.inputRegion = inputRegion;
            _corrector.outputRegion = outputRegion;
        }

        private void OnEvent(InputEventPtr ptr, InputDevice device)
        {
            // mapping sender coordinate system to receiver one.
            if (EnableInputPositionCorrection && device is Pointer && ptr.IsA<StateEvent>())
            {
                _corrector.Invoke(ptr, device);
            }
            else
            {
                onEvent?.Invoke(ptr, device);
            }
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
        public Observer(RTCDataChannel channel)
        {
            _channel = channel ?? throw new ArgumentNullException("channel is null");
        }
        public void OnNext(InputRemoting.Message value)
        {
            if (_channel.ReadyState != RTCDataChannelState.Open)
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
