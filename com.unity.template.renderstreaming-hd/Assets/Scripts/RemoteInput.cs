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
      Axis = 3
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

        static Dictionary<double, GamepadState> GamepadsStates;
        static Dictionary<double, int> GamepadsMap;
        public static Action<int> ActionButtonClick;

        static UnityEngine.Vector2Int m_prevMousePos;
        static bool m_isInitialized = false;

        public static void Initialize()
        {
            Keyboard = InputSystem.AddDevice<Keyboard>();
            RemoteMouse = InputSystem.AddDevice<Mouse>();
            EnhancedTouchSupport.Enable();
            RemoteTouch = InputSystem.AddDevice<Touchscreen>();
            GamepadsStates = new Dictionary<double, GamepadState>();
            GamepadsMap = new Dictionary<double, int>();
            m_isInitialized = true;
        }

//---------------------------------------------------------------------------------------------------------------------
        public static void Destroy()
        {
            InputSystem.RemoveDevice(RemoteMouse);
            InputSystem.RemoveDevice(Keyboard);
            EnhancedTouchSupport.Disable();
            InputSystem.RemoveDevice(RemoteTouch);
            var totalRemoteGamepads = Gamepad.all.Count - GamepadsMap.Count;
            for(int i = Gamepad.all.Count - 1; i >= totalRemoteGamepads; i-- )
            {
                InputSystem.RemoveDevice(Gamepad.all[i]);
            }
            GamepadsStates.Clear();
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
                        var id = BitConverter.ToDouble(bytes, 1);

                        if(!GamepadsMap.ContainsKey(id))
                        {
                            UE.Debug.Log("connecting " + id + " -> " + Gamepad.all.Count);
                            GamepadsMap[id] = Gamepad.all.Count;
                            InputSystem.AddDevice<Gamepad>();
                            GamepadsStates[id]=new GamepadState();
                        }

                        GamepadEventType gamepad_type = (GamepadEventType)bytes[9];
                        
                        switch (gamepad_type)
                        {
                            case GamepadEventType.ButtonDown:
                            case GamepadEventType.ButtonUp:
                            case GamepadEventType.ButtonPressed:
                                {
                                    var buttonIndex = bytes[10];
                                    var value = BitConverter.ToDouble(bytes, 11);
                                    ProcessGamepadButtonEvent(gamepad_type,(GamepadKeyCode) buttonIndex, id, value);
                                }
                                break;
                            case GamepadEventType.Axis:
                                {
                                    var buttonIndex = bytes[10];
                                    var x = BitConverter.ToDouble(bytes, 11);
                                    var y = BitConverter.ToDouble(bytes, 19);
                                    ProcessGamepadAxisEvent(x, y, (GamepadKeyCode) buttonIndex, id);
                                }
                                break;
                        }
                        InputSystem.QueueStateEvent(Gamepad.all[GamepadsMap[id]], GamepadsStates[id]);
                    }
                    break;
            }
        }

        public static void Reset()
        {
            if (!m_isInitialized)
                return;

            foreach(int index in GamepadsMap.Values)
            {
                InputSystem.QueueStateEvent(Gamepad.all[index], new GamepadState());
            }
            InputSystem.QueueStateEvent(RemoteMouse, new MouseState());
            InputSystem.QueueStateEvent(Keyboard, new KeyboardState());
            InputSystem.QueueStateEvent(RemoteTouch, new TouchState());
            InputSystem.Update();
        }

#region Gamepads Events
        static void ProcessGamepadButtonEvent(GamepadEventType state, GamepadKeyCode buttonIndex, double id, double value)
        {
            GamepadButton buttonToUpdate=GamepadButton.DpadUp;
            GamepadState gamepadState = GamepadsStates[id];
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
                    gamepadState.leftTrigger = (float)value;
                    break;
                case GamepadKeyCode.Button7:
                    buttonToUpdate = GamepadButton.RightTrigger;
                    gamepadState.rightTrigger = (float) value;
                    break;    
                default:
                    UE.Debug.Log("Unmapped button code: " + buttonIndex);
                    break;                   
            }
            GamepadsStates[id] = gamepadState.WithButton(buttonToUpdate, GamepadEventType.ButtonDown == state || GamepadEventType.ButtonPressed == state);
        }

        static void ProcessGamepadAxisEvent(double x, double y, GamepadKeyCode axisKeyCode, double id)
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
