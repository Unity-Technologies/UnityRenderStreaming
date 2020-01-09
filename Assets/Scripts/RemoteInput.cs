using System;
using UnityEngine.InputSystem;
using UnityEngine.InputSystem.LowLevel;
using UnityEngine.InputSystem.EnhancedTouch;

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
        public static Mouse RemoteMouse { get; private set; }
        public static Touchscreen RemoteTouch { get; private set; }
        public static Action<int> ActionButtonClick;

        static UnityEngine.Vector2Int m_prevMousePos;
        static bool m_isInitialized = false;

        public static void Initialize()
        {
            Keyboard = InputSystem.AddDevice<Keyboard>();
            RemoteMouse = InputSystem.AddDevice<Mouse>();
            EnhancedTouchSupport.Enable();
            RemoteTouch = InputSystem.AddDevice<Touchscreen>();
            m_isInitialized = true;
        }

//---------------------------------------------------------------------------------------------------------------------
        public static void Destroy()
        {
            InputSystem.RemoveDevice(RemoteMouse);
            InputSystem.RemoveDevice(Keyboard);
            EnhancedTouchSupport.Disable();
            InputSystem.RemoveDevice(RemoteTouch);
            RemoteMouse = null;
            Keyboard = null;
            RemoteTouch = null;
            m_isInitialized = false;
        }
//---------------------------------------------------------------------------------------------------------------------

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
                    break;
                case EventType.Mouse:
                    var deltaX = BitConverter.ToInt16(bytes, 1);
                    var deltaY = BitConverter.ToInt16(bytes, 3);
                    var button = bytes[5];
                    ProcessMouseMoveEvent(deltaX, deltaY, button);
                    break;
                case EventType.MouseWheel:
                    var scrollX = BitConverter.ToSingle(bytes, 1);
                    var scrollY = BitConverter.ToSingle(bytes, 5);
                    ProcessMouseWheelEvent(scrollX, scrollY);
                    break;
                case EventType.Touch:
                    var length = bytes[1];
                    var index = 2;
                    var touches = new TouchState[length];
                    for (int i = 0; i < length; i++)
                    {
                        const int INPUTSYSTEM_ZERO_ID_GUARD = 128; //ID 0 is reserved by inputsystem
                        int identifier = BitConverter.ToInt32(bytes, index) + INPUTSYSTEM_ZERO_ID_GUARD;
                        index += 4;
                        var phase = (UnityEngine.InputSystem.TouchPhase)bytes[index];
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
                            position = new UnityEngine.Vector2Int(pageX, pageY),
                            pressure = force
                        };
                    }
                    ProcessTouchMoveEvent(touches);
                    if (Touch.activeTouches.Count > length)
                    {
                        ChangeEndStateUnusedTouches(touches);
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
            if (!m_isInitialized)
                return;

            InputSystem.QueueStateEvent(RemoteMouse, new MouseState());
            InputSystem.QueueStateEvent(Keyboard, new KeyboardState());
            InputSystem.QueueStateEvent(RemoteTouch, new TouchState());
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

        static void ProcessMouseMoveEvent(short x, short y, byte button)
        {
            var position = new UnityEngine.Vector2Int(x, y);
            var delta = position - m_prevMousePos;
            InputSystem.QueueStateEvent(RemoteMouse, new MouseState { delta = delta, buttons = button });
            m_prevMousePos = position;
        }

        static void ProcessMouseWheelEvent(float scrollX, float scrollY)
        {
            InputSystem.QueueStateEvent(RemoteMouse, new MouseState { scroll = new UnityEngine.Vector2(scrollX, scrollY) });
        }

        static void ProcessTouchMoveEvent(TouchState[] touches)
        {
            for (var i = 0; i < touches.Length; i++)
            {
                InputSystem.QueueStateEvent(RemoteTouch, touches[i]);
            }
        }
        static void ChangeEndStateUnusedTouches(TouchState[] touches)
        {
            int touchCount = Touch.activeTouches.Count;
            for (var i = 0; i < touchCount; i++)
            {
                int touchId = Touch.activeTouches[i].touchId;
                if (!Array.Exists(touches, v => v.touchId == touchId))
                {
                    if (Touch.activeTouches[i].phase == TouchPhase.Ended)
                    {
                        continue;
                    }
                    InputSystem.QueueStateEvent(RemoteTouch, new TouchState
                    {
                        touchId = touchId,
                        phase = TouchPhase.Ended,
                        position = Touch.activeTouches[i].screenPosition
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
