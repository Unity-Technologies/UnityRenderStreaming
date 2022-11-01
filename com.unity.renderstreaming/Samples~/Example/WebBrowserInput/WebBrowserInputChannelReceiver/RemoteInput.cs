using System;
using UnityEngine.InputSystem;
using UnityEngine.InputSystem.Users;
using UnityEngine.InputSystem.LowLevel;
using UnityEngine.InputSystem.EnhancedTouch;
using UE = UnityEngine;
using System.Collections.Generic;
using System.Linq;

namespace Unity.RenderStreaming.Samples
{
    using UnityInputSystem = UnityEngine.InputSystem.InputSystem;

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
        Button9,
        Axis0Button,
        Axis1Button,
        DpadUp,
        DpadDown,
        DpadLeft,
        DpadRight,
        Axis0 = 100,
        Axis1
    }

    internal static class RemoteInputReceiver
    {
        private static readonly Dictionary<RemoteInput, uint> s_mapRemoteInputAndInputUserId;
        private static readonly List<RemoteInput> s_listRemoteInput;

        static RemoteInputReceiver()
        {
            s_mapRemoteInputAndInputUserId = new Dictionary<RemoteInput, uint>();
            s_listRemoteInput = new List<RemoteInput>();
        }

        internal static void Dispose()
        {
            s_mapRemoteInputAndInputUserId.Clear();
            s_listRemoteInput.Clear();
        }

        public static IReadOnlyList<RemoteInput> All()
        {
            return s_listRemoteInput;
        }

        public static RemoteInput Create()
        {
            InputUser user = InputUser.CreateUserWithoutPairedDevices();
            user = InputUser.PerformPairingWithDevice(UnityInputSystem.AddDevice<Mouse>(), user);
            user = InputUser.PerformPairingWithDevice(UnityInputSystem.AddDevice<Keyboard>(), user);
            user = InputUser.PerformPairingWithDevice(UnityInputSystem.AddDevice<Gamepad>(), user);
            user = InputUser.PerformPairingWithDevice(UnityInputSystem.AddDevice<Touchscreen>(), user);
            RemoteInput remoteInput = new RemoteInput(ref user);
            s_mapRemoteInputAndInputUserId.Add(remoteInput, user.id);
            s_listRemoteInput.Add(remoteInput);

            return remoteInput;
        }

        internal static void Delete(RemoteInput remoteInput)
        {
            if (remoteInput == null)
            {
                throw new ArgumentException("The instance of argument is null");
            }
            bool found = s_mapRemoteInputAndInputUserId.TryGetValue(remoteInput, out uint userId);
            if (!found)
            {
                throw new ArgumentException("The instance of argument is not found");
            }
            InputUser user = InputUser.all.First(_user => _user.id == userId);
            var arrayDeviceId = user.pairedDevices.Select(device => device.deviceId).ToArray();
            user.UnpairDevicesAndRemoveUser();
            foreach (var deviceId in arrayDeviceId)
            {
                UnityInputSystem.RemoveDevice(UnityInputSystem.GetDeviceById(deviceId));
            }
            s_mapRemoteInputAndInputUserId.Remove(remoteInput);
            s_listRemoteInput.Remove(remoteInput);
        }
    }

    public interface IInput
    {
        Mouse RemoteMouse { get; }
        Keyboard RemoteKeyboard { get; }
        Touchscreen RemoteTouchscreen { get; }
        Gamepad RemoteGamepad { get; }
    }

    internal class RemoteInput : IInput, IDisposable
    {
        private GamepadState m_gamepadState;

        public Mouse RemoteMouse { get; }
        public Keyboard RemoteKeyboard { get; }
        public Touchscreen RemoteTouchscreen { get; }
        public Gamepad RemoteGamepad { get; }

        public Action<int> ActionButtonClick;

        private UnityEngine.Vector2Int m_prevMousePos;
        private KeyboardState m_keyboardState = new KeyboardState();
        private bool disposed;

        internal RemoteInput(ref InputUser user)
        {
            RemoteMouse = user.pairedDevices.FirstOrDefault(device => device is Mouse) as Mouse;
            RemoteKeyboard = user.pairedDevices.FirstOrDefault(device => device is Keyboard) as Keyboard;
            RemoteTouchscreen = user.pairedDevices.FirstOrDefault(device => device is Touchscreen) as Touchscreen;
            RemoteGamepad = user.pairedDevices.FirstOrDefault(device => device is Gamepad) as Gamepad;
        }

        ~RemoteInput()
        {
            Dispose();
        }

        public void Dispose()
        {
            if (this.disposed)
            {
                return;
            }
            RemoteInputReceiver.Delete(this);

            this.disposed = true;
            GC.SuppressFinalize(this);
        }

//---------------------------------------------------------------------------------------------------------------------

        public void ProcessInput(byte[] bytes)
        {
            if (bytes == null)
                throw new ArgumentNullException();
            if(bytes.Length == 0)
                throw new ArgumentException("byte length is zero");

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
                        GamepadEventType gamepadEventType = (GamepadEventType)bytes[1];
                        
                        switch (gamepadEventType)
                        {
                            case GamepadEventType.ButtonDown:
                            case GamepadEventType.ButtonUp:
                            case GamepadEventType.ButtonPressed:
                                {
                                    var buttonIndex = bytes[2];
                                    var value = BitConverter.ToDouble(bytes, 3);
                                    ProcessGamepadButtonEvent(gamepadEventType, (GamepadKeyCode) buttonIndex, value);
                                }
                                break;
                            case GamepadEventType.Axis:
                                {
                                    var buttonIndex = bytes[2];
                                    var x = BitConverter.ToDouble(bytes, 3);
                                    var y = BitConverter.ToDouble(bytes, 11);
                                    ProcessGamepadAxisEvent(x, y, (GamepadKeyCode) buttonIndex);
                                }
                                break;
                        }
                        UnityInputSystem.QueueStateEvent(RemoteGamepad, m_gamepadState);
                    }
                    break;
            }
        }

        #region Gamepads Events
        void ProcessGamepadButtonEvent(GamepadEventType state, GamepadKeyCode buttonIndex, double value)
        {
            GamepadButton buttonToUpdate = GamepadButton.DpadUp;
            GamepadState gamepadState = m_gamepadState;
            switch(buttonIndex)
            {
                case GamepadKeyCode.DpadUp:
                    buttonToUpdate = GamepadButton.DpadUp;
                    break;
                case GamepadKeyCode.DpadDown:
                    buttonToUpdate = GamepadButton.DpadDown;
                    break;
                case GamepadKeyCode.DpadLeft:
                    buttonToUpdate = GamepadButton.DpadLeft;
                    break;
                case GamepadKeyCode.DpadRight:
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
                case GamepadKeyCode.Button4:
                    buttonToUpdate = GamepadButton.LeftShoulder;
                    break;
                case GamepadKeyCode.Button5:
                    buttonToUpdate = GamepadButton.RightShoulder;
                    break;
                case GamepadKeyCode.Button6:
                    buttonToUpdate = GamepadButton.LeftTrigger;
                    gamepadState.leftTrigger = (float)value;
                    break;
                case GamepadKeyCode.Button7:
                    buttonToUpdate = GamepadButton.RightTrigger;
                    gamepadState.rightTrigger = (float) value;
                    break;
                case GamepadKeyCode.Axis0Button:
                    buttonToUpdate = GamepadButton.LeftStick;
                    break;
                case GamepadKeyCode.Axis1Button:
                    buttonToUpdate = GamepadButton.RightStick;
                    break;    
                default:
                    UE.Debug.Log("Unmapped button code: " + buttonIndex);
                    break;                   
            }
            m_gamepadState = gamepadState.WithButton(buttonToUpdate, GamepadEventType.ButtonDown == state || GamepadEventType.ButtonPressed == state);
        }

        void ProcessGamepadAxisEvent(double x, double y, GamepadKeyCode axisKeyCode)
        {
            GamepadState gamepadState = m_gamepadState;
            if(axisKeyCode == GamepadKeyCode.Axis0)
                gamepadState.leftStick = new UE.Vector2((float)x, (float)y);
            if(axisKeyCode == GamepadKeyCode.Axis1)
                gamepadState.rightStick = new UE.Vector2((float)x, (float)y);

            m_gamepadState = gamepadState;
        }
#endregion
        void ProcessKeyEvent(KeyboardEventType state, bool repeat, byte keyCode, char character)
        {
            switch(state)
            {
                case KeyboardEventType.KeyDown:
                    if (!repeat)
                    {
                        m_keyboardState.Set((Key)keyCode, true);
                        UnityInputSystem.QueueStateEvent(RemoteKeyboard, m_keyboardState);
                    }
                    if(character != 0)
                    {
                        UnityInputSystem.QueueTextEvent(RemoteKeyboard, character);
                    }
                    break;
                case KeyboardEventType.KeyUp:
                    m_keyboardState.Set((Key)keyCode, false);
                    UnityInputSystem.QueueStateEvent(RemoteKeyboard, m_keyboardState);
                    break;
            }
        }

        void ProcessMouseMoveEvent(short x, short y, byte button)
        {
            UnityEngine.Vector2Int pos = new UnityEngine.Vector2Int(x, y);
            UnityEngine.Vector2Int delta = pos - m_prevMousePos;
            UnityInputSystem.QueueStateEvent(RemoteMouse, new MouseState {
                position = pos,
                delta = delta,
                buttons = button
            });
            m_prevMousePos = pos;
        }

        void ProcessMouseWheelEvent(float scrollX, float scrollY)
        {
            UnityInputSystem.QueueStateEvent(RemoteMouse, new MouseState { scroll = new UnityEngine.Vector2(scrollX, scrollY) });
        }

        void ProcessTouchMoveEvent(TouchState[] touches)
        {
            for (var i = 0; i < touches.Length; i++)
            {
                UnityInputSystem.QueueStateEvent(RemoteTouchscreen, touches[i]);
            }
        }
        void ChangeEndStateUnusedTouches(TouchState[] touches)
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
                    UnityInputSystem.QueueStateEvent(RemoteTouchscreen, new TouchState
                    {
                        touchId = touchId,
                        phase = TouchPhase.Ended,
                        position = Touch.activeTouches[i].screenPosition
                    });
                }
            }
        }

        void ProcessButtonClickEvent(int elementId)
        {
            ActionButtonClick?.Invoke(elementId);
        }
    }
}
