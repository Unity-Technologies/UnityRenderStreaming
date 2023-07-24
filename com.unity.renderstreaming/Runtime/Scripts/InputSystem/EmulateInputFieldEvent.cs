using System.Collections.Generic;
#if URS_USE_TEXTMESHPRO
using TMPro;
#endif
using UnityEngine;
using UnityEngine.UI;
using UnityEngine.InputSystem;
using UnityEngine.InputSystem.Controls;
using UnityEngine.InputSystem.LowLevel;

namespace Unity.RenderStreaming.InputSystem
{
    /// <summary>
    /// This partial class is for workaround to support Unity UI InputField.
    /// </summary>
    partial class Receiver
    {
        private static readonly Dictionary<int, KeyCode> s_KeyMap = new Dictionary<int, KeyCode>()
        {
            { (int)Key.Backspace, KeyCode.Backspace },
            { (int)Key.Tab, KeyCode.Tab },
            { (int)Key.Enter, KeyCode.Return },
            { (int)Key.Space, KeyCode.Space },
            { (int)Key.Comma, KeyCode.Comma },
            { (int)Key.Minus, KeyCode.Minus },
            { (int)Key.Period, KeyCode.Period },
            { (int)Key.Slash, KeyCode.Slash },
            { (int)Key.Digit0, KeyCode.Alpha0 },
            { (int)Key.Digit1, KeyCode.Alpha1 },
            { (int)Key.Digit2, KeyCode.Alpha2 },
            { (int)Key.Digit3, KeyCode.Alpha3 },
            { (int)Key.Digit4, KeyCode.Alpha4 },
            { (int)Key.Digit5, KeyCode.Alpha5 },
            { (int)Key.Digit6, KeyCode.Alpha6 },
            { (int)Key.Digit7, KeyCode.Alpha7 },
            { (int)Key.Digit8, KeyCode.Alpha8 },
            { (int)Key.Digit9, KeyCode.Alpha9 },
            { (int)Key.Semicolon, KeyCode.Semicolon },
            { (int)Key.Equals, KeyCode.Equals },
            { (int)Key.LeftBracket, KeyCode.LeftBracket },
            { (int)Key.Backslash, KeyCode.Backslash },
            { (int)Key.RightBracket, KeyCode.RightBracket },
            { (int)Key.Backquote, KeyCode.BackQuote },
            { (int)Key.Quote, KeyCode.Quote },
            { (int)Key.A, KeyCode.A },
            { (int)Key.B, KeyCode.B },
            { (int)Key.C, KeyCode.C },
            { (int)Key.D, KeyCode.D },
            { (int)Key.E, KeyCode.E },
            { (int)Key.F, KeyCode.F },
            { (int)Key.G, KeyCode.G },
            { (int)Key.H, KeyCode.H },
            { (int)Key.I, KeyCode.I },
            { (int)Key.J, KeyCode.J },
            { (int)Key.K, KeyCode.K },
            { (int)Key.L, KeyCode.L },
            { (int)Key.M, KeyCode.M },
            { (int)Key.N, KeyCode.N },
            { (int)Key.O, KeyCode.O },
            { (int)Key.P, KeyCode.P },
            { (int)Key.Q, KeyCode.Q },
            { (int)Key.R, KeyCode.R },
            { (int)Key.S, KeyCode.S },
            { (int)Key.T, KeyCode.T },
            { (int)Key.U, KeyCode.U },
            { (int)Key.V, KeyCode.V },
            { (int)Key.W, KeyCode.W },
            { (int)Key.X, KeyCode.X },
            { (int)Key.Y, KeyCode.Y },
            { (int)Key.Z, KeyCode.Z },
            { (int)Key.F1, KeyCode.F1 },
            { (int)Key.F2, KeyCode.F2 },
            { (int)Key.F3, KeyCode.F3 },
            { (int)Key.F4, KeyCode.F4 },
            { (int)Key.F5, KeyCode.F5 },
            { (int)Key.F6, KeyCode.F6 },
            { (int)Key.F7, KeyCode.F7 },
            { (int)Key.F8, KeyCode.F8 },
            { (int)Key.F9, KeyCode.F9 },
            { (int)Key.F10, KeyCode.F10 },
            { (int)Key.F11, KeyCode.F11 },
            { (int)Key.F12, KeyCode.F12 },
            { (int)Key.None, KeyCode.None },
            { (int)Key.LeftArrow, KeyCode.LeftArrow },
            { (int)Key.RightArrow, KeyCode.RightArrow },
            { (int)Key.UpArrow, KeyCode.UpArrow },
            { (int)Key.DownArrow, KeyCode.DownArrow },
            { (int)Key.LeftShift, KeyCode.LeftShift },
            { (int)Key.RightShift, KeyCode.RightShift },
            { (int)Key.Delete, KeyCode.Delete },
            { (int)Key.Escape, KeyCode.Escape },
            { (int)Key.LeftAlt, KeyCode.LeftAlt },
            { (int)Key.RightAlt, KeyCode.RightAlt },
            { (int)Key.LeftApple, KeyCode.LeftApple },
            { (int)Key.RightApple, KeyCode.RightApple }
        };

        interface IInputField
        {
            void ProcessEvent(Event e);
            void ForceLabelUpdate();
            void AppendText(char character);
        }

        class UGUIInputField : IInputField
        {
            InputField m_field;
            public UGUIInputField(InputField field)
            {
                m_field = field;
            }

            public void ProcessEvent(Event e)
            {
                m_field.ProcessEvent(e);
            }
            public void ForceLabelUpdate()
            {
                m_field.ForceLabelUpdate();
            }
            public void AppendText(char character)
            {
                m_field.text += character;
            }
        }

#if URS_USE_TEXTMESHPRO
        class TMProInputField : IInputField
        {
            TMP_InputField m_field;
            public TMProInputField(TMP_InputField field)
            {
                m_field = field;
            }

            public void ProcessEvent(Event e)
            {
                m_field.ProcessEvent(e);
            }
            public void ForceLabelUpdate()
            {
                m_field.ForceLabelUpdate();
            }
            public void AppendText(char character)
            {
                m_field.text += character;
            }
        }
#endif

        IInputField FindInputField(GameObject obj)
        {
            var field = obj.GetComponent<InputField>();
            if (field != null)
                return new UGUIInputField(field);

#if URS_USE_TEXTMESHPRO
            var tmpField = obj.GetComponent<TMP_InputField>();
            if (tmpField != null)
                return new TMProInputField(tmpField);
#endif
            return null;
        }

        (EventModifiers, KeyCode) GetEventModifiersAndKeyCode(InputEventPtr ptr)
        {
            EventModifiers modifiers = EventModifiers.None;
            KeyCode keyCode = KeyCode.None;
            foreach (var control in ptr.GetAllButtonPresses())
            {
                if (control is KeyControl keyControl)
                {
                    var key = keyControl.keyCode;
                    if (key == Key.LeftShift || key == Key.RightShift)
                    {
                        modifiers |= EventModifiers.Shift;
                    }
                    else if (key == Key.LeftCtrl || key == Key.RightCtrl)
                    {
                        modifiers |= EventModifiers.Control;
                    }
                    else if (key == Key.LeftAlt || key == Key.RightAlt)
                    {
                        modifiers |= EventModifiers.Alt;
                    }
                    else if (key == Key.LeftCommand || key == Key.RightCommand)
                    {
                        modifiers |= EventModifiers.Command;
                    }
                    else if (key == Key.CapsLock)
                    {
                        modifiers |= EventModifiers.CapsLock;
                    }
                    else if (s_KeyMap.TryGetValue((int)key, out var value))
                    {
                        keyCode = value;
                    }
                }
            }
            return (modifiers, keyCode);
        }

        unsafe Event CreateEvent(InputEventPtr ptr)
        {
            var (modifiers, keyCode) = GetEventModifiersAndKeyCode(ptr);

            if (ptr.type == TextEvent.Type)
            {
                var textEventPtr = (TextEvent*)ptr.ToPointer();
                var utf32Char = textEventPtr->character;
                if (utf32Char >= 0x10000)
                {
                    // todo: not supported multibyte character.
                    return null;
                }

                return new Event
                {
                    type = EventType.KeyDown,
                    character = (char)utf32Char,
                    keyCode = keyCode,
                    modifiers = modifiers
                };
            }

            return new Event
            {
                type = EventType.KeyDown,
                keyCode = keyCode,
                modifiers = modifiers
            };
        }

        private void EmulateInputFieldEvent(InputEventPtr ptr)
        {
            var obj = UnityEngine.EventSystems.EventSystem.current?.currentSelectedGameObject;
            if (obj == null)
                return;

            var field = FindInputField(obj);
            if (field == null)
                return;
            Event e = CreateEvent(ptr);
            if (e != null)
            {
                field.ProcessEvent(e);
                field.ForceLabelUpdate();
            }
        }
    }
}
