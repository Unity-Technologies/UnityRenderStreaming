using System;
using UnityEngine;
using UnityEngine.InputSystem;
using UnityEngine.InputSystem.LowLevel;

namespace Unity.RenderStreaming
{
    enum KeyEventType
    {
        KeyDown = 0,
        KeyUp,
        KeyPress
    }
    enum EventType
    {
        Keyboard = 0,
        MouseDown = 2,
        MouseMove = 4,
        MouseWheel = 5,
        Touch = 6
    }

    public static class RemoteInput
    {
        public static Keyboard Keyboard { get; private set; }
        public static Mouse Mouse { get; private set; }
        public static Touchscreen Touch { get; private set; }

        static TDevice GetOrAddDevice<TDevice>() where TDevice : InputDevice
        {
            var device = InputSystem.GetDevice<TDevice>();
            if(device != null)
            {
                return device;
            }
            return InputSystem.AddDevice<TDevice>();
        }

        static RemoteInput()
        {
            Keyboard = GetOrAddDevice<Keyboard>();
            Mouse = GetOrAddDevice<Mouse>();
            Touch = GetOrAddDevice<Touchscreen>();
        }

        public static void ProcessInput(byte[] bytes)
        {
            switch ((EventType)bytes[0])
            {
                case EventType.Keyboard:
                    ProcessKeyEvent((char)bytes[1]);
                    break;
                case EventType.MouseDown:
                    ProcessMouseDownEvent(bytes[1]);
                    break;
                case EventType.MouseMove:
                    var deltaX = BitConverter.ToInt16(bytes, 1);
                    var deltaY = BitConverter.ToInt16(bytes, 3);
                    ProcessMouseMoveEvent(deltaX, deltaY, bytes[5]);
                    break;
                case EventType.Touch:
                    var phase = (PointerPhase)bytes[1];
                    var length = bytes[2];
                    var index = 3;
                    for (int i = 0; i < length; i++)
                    {
                        var pageX = BitConverter.ToInt16(bytes, index);
                        var pageY = BitConverter.ToInt16(bytes, index+2);
                        var force = BitConverter.ToSingle(bytes, index+4);
                        ProcessTouchMoveEvent(i, phase, pageX, pageY, force);
                        index += 8;
                    }
                    break;

            }
        }

        static void ProcessKeyEvent(char keyCode)
        {
            InputSystem.QueueStateEvent(Keyboard, new KeyboardState((Key)keyCode));
            InputSystem.QueueTextEvent(Keyboard, keyCode);
            InputSystem.Update();
        }

        static void ProcessMouseMoveEvent(short deltaX, short deltaY, byte button)
        {
            InputSystem.QueueStateEvent(Mouse, new MouseState { delta = new Vector2Int(deltaX, deltaY), buttons = button });
            InputSystem.Update();
        }

        static void ProcessMouseDownEvent(byte button)
        {
            InputSystem.QueueStateEvent(Mouse, new MouseState { buttons = button });
            InputSystem.Update();
        }

        static void ProcessTouchMoveEvent(int index, PointerPhase phase, short pageX, short pageY, float force)
        {
            InputSystem.QueueDeltaStateEvent(Touch.allTouchControls[index],
                new TouchState
                {
                    touchId = index,
                    phase = phase,
                    position = new Vector2Int(pageX, pageY),
                    pressure = force
                });
            InputSystem.Update();
        }
    }
}
