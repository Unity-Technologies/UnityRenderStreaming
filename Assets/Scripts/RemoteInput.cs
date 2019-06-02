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
        TouchMove = 6
    }

    public static class RemoteInput
    {
        static Keyboard keyboard;
        static Mouse mouse;
        static Touchscreen touch;

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
            keyboard = GetOrAddDevice<Keyboard>();
            mouse = GetOrAddDevice<Mouse>();
            touch = GetOrAddDevice<Touchscreen>();
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
                case EventType.TouchMove:
                    var pageX = BitConverter.ToInt16(bytes, 1);
                    var pageY = BitConverter.ToInt16(bytes, 3);
                    ProcessTouchMoveEvent(pageX, pageY);
                    break;

            }
        }

        static void ProcessKeyEvent(char keyCode)
        {
            InputSystem.QueueStateEvent(keyboard, new KeyboardState((Key)keyCode));
            InputSystem.QueueTextEvent(keyboard, keyCode);
            InputSystem.Update();
        }

        static void ProcessMouseMoveEvent(short deltaX, short deltaY, byte button)
        {
            InputSystem.QueueStateEvent(mouse, new MouseState { delta = new Vector2Int(deltaX, deltaY), buttons = button });
            InputSystem.Update();
        }

        static void ProcessMouseDownEvent(byte button)
        {
            InputSystem.QueueStateEvent(mouse, new MouseState { buttons = button });
            InputSystem.Update();
        }

        static void ProcessTouchMoveEvent(short pageX, short pageY)
        {
            InputSystem.QueueStateEvent(touch, new TouchState { position = new Vector2Int(pageX, pageY) });
            InputSystem.Update();
        }
    }
}
