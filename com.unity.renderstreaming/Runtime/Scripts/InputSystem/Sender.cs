using System;
using System.Collections.Generic;
using Unity.WebRTC;
using Unity.Collections.LowLevel.Unsafe;
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
                return InputSystem.ListLayouts();
            }
        }

        public bool EnableCorrectPointerPosition { set; get; }

        public Vector2Int FrameSize { set; get; }

        public Rect Region { set; get; }


        Vector2 Translate(ref Vector2 position)
        {
            return Rect.PointToNormalized(Region, position) * FrameSize;
        }

        unsafe void PointerMap(StateEvent* data, InputDevice device)
        {
            switch (device)
            {
                case Mouse mouse:
                    MouseState* mouseState = (MouseState*)data->state;
                    mouseState->position = Translate(ref mouseState->position);
                    break;
            }
        }

        unsafe void OnPointerEvent(ref InputEventPtr ptr, InputDevice device)
        {
            // Allocate memory and copy InputEventPtr
            InputEventPtr dst = (InputEventPtr)
                UnsafeUtility.Malloc(ptr.sizeInBytes, 4, Collections.Allocator.Temp);
            UnsafeUtility.MemCpy(dst, ptr, ptr.sizeInBytes);

            // Mapping 
            PointerMap((StateEvent*)dst.data, device);

            onEvent?.Invoke(dst, device);

            // Free memory
            UnsafeUtility.Free(dst, Collections.Allocator.Temp);
        }

        private void OnEvent(InputEventPtr ptr, InputDevice device)
        {
            // mapping sender coordinate system to receiver one.
            if (EnableCorrectPointerPosition && device is Pointer && ptr.IsA<StateEvent>())
            {
                OnPointerEvent(ref ptr, device);
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
