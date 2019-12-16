using System;
using UnityEngine.InputSystem;
using UnityEngine.InputSystem.LowLevel;
using UnityEngine.InputSystem.EnhancedTouch;
using UE = UnityEngine;
using System.Collections.Generic;

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
        ButtonClick = 4,
        Gamepad = 5,
    }

    enum GamepadEventType {
      ButtonUp = 0,
      ButtonDown = 1,
      ButtonPressed = 2,
      Axis = 3,
    }

    enum GamepadKeyCode {
        Button0 = 0,
        Button1,
        Button2,
        Button3,
        Button4,
        Button5,
        Button6,
        Button7,
        Button8,
        button9,
        axis0Button,
        axis1Button,
        dpadUp,
        dpadDown,
        dpadLeft,
        dpadRight,
        axis0 = 100,
        axis1
        
    }

    public static class RemoteInput
    {
        public static Keyboard Keyboard { get; private set; }
        public static Mouse RemoteMouse { get; private set; }
        public static Touchscreen RemoteTouch { get; private set; }

        static List<GamepadState> GamepadsStates;
        static int RemoteGamepadsIndexStart;
        public static Action<int> ActionButtonClick;

        static UnityEngine.Vector2Int m_prevMousePos;
        static bool m_isInitialized = false;

        public static void Initialize()
        {
            Keyboard = InputSystem.AddDevice<Keyboard>();
            RemoteMouse = InputSystem.AddDevice<Mouse>();
            EnhancedTouchSupport.Enable();
            RemoteTouch = InputSystem.AddDevice<Touchscreen>();
            GamepadsStates = new List<GamepadState>();
            RemoteGamepadsIndexStart = Gamepad.all.Count;
            m_isInitialized = true;
        }

//---------------------------------------------------------------------------------------------------------------------
        public static void Destroy()
        {
            InputSystem.RemoveDevice(RemoteMouse);
            InputSystem.RemoveDevice(Keyboard);
            EnhancedTouchSupport.Disable();
            InputSystem.RemoveDevice(RemoteTouch);
            for(var i = Gamepad.all.Count-1; i >= RemoteGamepadsIndexStart; i--)
            {
                InputSystem.RemoveDevice(Gamepad.all[i]);
            }
            GamepadsStates.Clear();
            RemoteGamepadsIndexStart = 0;
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
                    {
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
                    }
                    
                    break;
                case EventType.ButtonClick:
                    var elementId = BitConverter.ToInt16(bytes, 1);
                    ProcessButtonClickEvent(elementId);
                    break;
                case EventType.Gamepad:
                    {
                        var id = (int)bytes[1];

                        if(id + RemoteGamepadsIndexStart >= Gamepad.all.Count)
                        {
                            for(int i = Gamepad.all.Count; i <= id + RemoteGamepadsIndexStart; i++)
                            {
                                InputSystem.AddDevice<Gamepad>();
                                GamepadsStates.Add(new GamepadState());
                            }
                        }

                        GamepadEventType gamepad_type = (GamepadEventType)bytes[2];
                        
                        switch (gamepad_type)
                        {
                            case GamepadEventType.ButtonDown:
                            case GamepadEventType.ButtonUp:
                            case GamepadEventType.ButtonPressed:
                                {
                                    var buttonIndex = bytes[3];
                                    ProcessGamepadButtonEvent(gamepad_type,(GamepadKeyCode) buttonIndex, id);
                                }
                                break;
                            case GamepadEventType.Axis:
                                {
                                    var buttonIndex = bytes[3];
                                    var x = BitConverter.ToDouble(bytes, 4);
                                    var y = BitConverter.ToDouble(bytes, 12);
                                    ProcessGamepadAxisEvent(x, y, (GamepadKeyCode) buttonIndex, id);
                                }
                                break;
                        }
                        InputSystem.QueueStateEvent(Gamepad.all[id+RemoteGamepadsIndexStart], GamepadsStates[id]);
                    }
                    break;
            }
        }


        public static void Reset()
        {
            if (!m_isInitialized)
                return;

            for(var i = RemoteGamepadsIndexStart; i < Gamepad.all.Count; i++)
            {
                InputSystem.QueueStateEvent(Gamepad.all[i], new GamepadState());
            }
            InputSystem.QueueStateEvent(RemoteMouse, new MouseState());
            InputSystem.QueueStateEvent(Keyboard, new KeyboardState());
            InputSystem.QueueStateEvent(RemoteTouch, new TouchState());
            InputSystem.Update();
        }

#region Gamepads Events
        static void ProcessGamepadButtonEvent(GamepadEventType state, GamepadKeyCode buttonIndex, int id)
        {
            GamepadButton buttonToUpdate=GamepadButton.DpadUp;
            switch(buttonIndex)
            {
                case GamepadKeyCode.dpadUp:
                    buttonToUpdate = GamepadButton.DpadUp;
                    break;
                case GamepadKeyCode.dpadDown:
                    buttonToUpdate = GamepadButton.DpadDown;
                    break;
                case GamepadKeyCode.dpadLeft:
                    buttonToUpdate = GamepadButton.DpadLeft;
                    break;
                case GamepadKeyCode.dpadRight:
                    buttonToUpdate = GamepadButton.DpadRight;
                    break;
                case GamepadKeyCode.Button0:
                    buttonToUpdate = GamepadButton.B;
                    break;
                case GamepadKeyCode.Button1:
                    buttonToUpdate = GamepadButton.A;
                    break;
                case GamepadKeyCode.Button2:
                    buttonToUpdate = GamepadButton.Y;
                    break;
                case GamepadKeyCode.Button3:
                    buttonToUpdate = GamepadButton.X;
                    break;
                case GamepadKeyCode.Button6:
                    buttonToUpdate = GamepadButton.LeftTrigger;
                    break;
                case GamepadKeyCode.Button7:
                    buttonToUpdate = GamepadButton.RightTrigger;
                    break;    
                default:
                    UE.Debug.Log("Unmapped button code: " + buttonIndex);
                    break;                   
            }
            GamepadsStates[id] = GamepadsStates[id].WithButton(buttonToUpdate, GamepadEventType.ButtonDown == state || GamepadEventType.ButtonPressed == state);
        }

        static void ProcessGamepadAxisEvent(double x, double y, GamepadKeyCode axisKeyCode, int id)
        {
            GamepadState gamepadState = GamepadsStates[id];
            if(axisKeyCode == GamepadKeyCode.axis0)
                gamepadState.leftStick = new UE.Vector2((float)x, (float)y);
            if(axisKeyCode == GamepadKeyCode.axis1)
                gamepadState.rightStick = new UE.Vector2((float)x, (float)y);

            GamepadsStates[id]= gamepadState;
        }
#endregion
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
            UnityEngine.Vector2Int pos = new UnityEngine.Vector2Int(x, y);
            UnityEngine.Vector2Int delta = pos- m_prevMousePos;
            InputSystem.QueueStateEvent(RemoteMouse, new MouseState {
                position = pos,
                delta = delta,
                buttons = button
            });
            m_prevMousePos = pos;
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
