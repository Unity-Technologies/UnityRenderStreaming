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
        Mouse = 1,
    }

    public static class RemoteInput
    {
        static Keyboard keyboard;
        static Mouse mouse;
        static RemoteInput()
        {
            keyboard = InputSystem.AddDevice<Keyboard>();
            mouse = InputSystem.AddDevice<Mouse>();
        }

        public static void ProcessInput(byte[] bytes)
        {
            switch ((EventType)bytes[0])
            {
                case EventType.Keyboard:
                    ProcessKeyEvent((Key)bytes[1]);
                    break;
                case EventType.Mouse:
                    var deltaX = BitConverter.ToInt16(bytes, 1);
                    var deltaY = BitConverter.ToInt16(bytes, 3);
                    ProcessMouseMoveEvent(deltaX, deltaY);
                    break;
            }
        }

        static void ProcessKeyEvent(Key keyCode)
        {
            InputSystem.QueueStateEvent(keyboard, new KeyboardState(keyCode));
            InputSystem.Update();
        }

        static void ProcessMouseMoveEvent(short deltaX, short deltaY)
        {
            InputSystem.QueueStateEvent(mouse, new MouseState { delta = new Vector2Int(deltaX, deltaY)});
            InputSystem.Update();
        }
    }
}
