using System;
using UnityEngine;
using UnityEngine.InputSystem;
using UnityEngine.InputSystem.LowLevel;

namespace Unity.RenderStreaming
{
    enum KeyboardEventType
    {
        KeyUp = 0,
        KeyDown = 1,
    }
    enum EventType
    {
        Keyboard = 0,
        Mouse = 1,
        MouseWheel = 2,
        Touch = 3,
        ButtonClick = 4
    }

    public static class RemoteInput
    {
        public static Keyboard Keyboard { get; private set; }
        public static Mouse Mouse { get; private set; }
        public static Touchscreen Touch { get; private set; }
        public static Action<int> ActionButtonClick;

        static TDevice GetOrAddDevice<TDevice>() where TDevice : InputDevice
        {
            var device = InputSystem.GetDevice<TDevice>();
            if(device != null)
            {
                return device;
            }
            return InputSystem.AddDevice<TDevice>();
        }

        public static void Initialize()
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
                    var type = (KeyboardEventType)bytes[1];
                    var repeat = bytes[2] == 1;
                    var key = bytes[3];
                    var character = (char)bytes[4];
                    ProcessKeyEvent(type, repeat, key, character);
                    InputSystem.Update();
                    break;
                case EventType.Mouse:
                    var deltaX = BitConverter.ToInt16(bytes, 1);
                    var deltaY = BitConverter.ToInt16(bytes, 3);
                    var button = bytes[5];
                    ProcessMouseMoveEvent(deltaX, deltaY, button);
                    InputSystem.Update();
                    break;
                case EventType.MouseWheel:
                    var scrollX = BitConverter.ToSingle(bytes, 1);
                    var scrollY = BitConverter.ToSingle(bytes, 5);
                    ProcessMouseWheelEvent(scrollX, scrollY);
                    InputSystem.Update();
                    break;
                case EventType.Touch:
                    var length = bytes[1];
                    var index = 2;
                    var touches = new TouchState[length];
                    for (int i = 0; i < length; i++)
                    {
                        var identifier = BitConverter.ToInt32(bytes, index);
                        index += 4;
                        var phase = (PointerPhase)bytes[index];
                        index += 1;
                        var pageX = BitConverter.ToInt16(bytes, index);
                        index += 2;
                        var pageY = BitConverter.ToInt16(bytes, index);
                        index += 2;
                        var force = BitConverter.ToSingle(bytes, index);
                        index += 4;
                        touches[i] = new TouchState
                        {
                            touchId = identifier,
                            phase = phase,
                            position = new Vector2Int(pageX, pageY),
                            pressure = force
                        };
                    }
                    ProcessTouchMoveEvent(touches);
                    InputSystem.Update();
                    if (Touchscreen.current.activeTouches.Count > length)
                    {
                        ChangeEndStateUnusedTouches(touches);
                        InputSystem.Update();
                    }
                    break;
                case EventType.ButtonClick:
                    var elementId = BitConverter.ToInt16(bytes, 1);
                    ProcessButtonClickEvent(elementId);
                    break;
            }
        }

        public static void Reset()
        {
            InputSystem.QueueStateEvent(Mouse, new MouseState());
            InputSystem.QueueStateEvent(Keyboard, new KeyboardState());
            InputSystem.QueueStateEvent(Touch, new TouchState());
            InputSystem.Update();
        }

        static void ProcessKeyEvent(KeyboardEventType state, bool repeat, byte keyCode, char character)
        {
            switch(state)
            {
                case KeyboardEventType.KeyDown:
                    if (!repeat)
                    {
                        InputSystem.QueueStateEvent(Keyboard, new KeyboardState((Key)keyCode));
                    }
                    if(character != 0)
                    {
                        InputSystem.QueueTextEvent(Keyboard, character);
                    }
                    break;
                case KeyboardEventType.KeyUp:
                    InputSystem.QueueStateEvent(Keyboard, new KeyboardState());
                    break;
            }
        }

        static void ProcessMouseMoveEvent(short deltaX, short deltaY, byte button)
        {
            InputSystem.QueueStateEvent(Mouse, new MouseState { delta = new Vector2Int(deltaX, deltaY), buttons = button });
        }

        static void ProcessMouseWheelEvent(float scrollX, float scrollY)
        {
            InputSystem.QueueStateEvent(Mouse, new MouseState { scroll = new Vector2(scrollX, scrollY) });
        }

        static void ProcessTouchMoveEvent(TouchState[] touches)
        {
            for (var i = 0; i < touches.Length; i++)
            {
                InputSystem.QueueStateEvent(Touch, touches[i]);
            }
        }
        static void ChangeEndStateUnusedTouches(TouchState[] touches)
        {
            for (var i = 0; i < Touchscreen.current.activeTouches.Count; i++)
            {
                var touchId = Touchscreen.current.activeTouches[i].touchId.ReadValue();
                if (!Array.Exists(touches, v => v.touchId == touchId))
                {
                    InputSystem.QueueStateEvent(Touch, new TouchState
                    {
                        touchId = touchId,
                        phase = PointerPhase.Ended
                    });
                }
            }
        }

        static void ProcessButtonClickEvent(int elementId)
        {
            if (ActionButtonClick != null)
            {
                ActionButtonClick(elementId);
            }
        }
    }
}
