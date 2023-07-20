using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using UnityEngine.InputSystem;
using UnityEngine.InputSystem.Controls;
using UnityEngine.InputSystem.LowLevel;
#if URS_USE_TEXTMESHPRO
using TMPro;
#endif

namespace Unity.RenderStreaming.InputSystem
{
    /// <summary>
    /// This partial class is for workaround to support Unity UI InputField.
    /// </summary>
    partial class Receiver
    {
        private static readonly Dictionary<Key, KeyCode> s_KeyMap = new()
        {
            { Key.Backspace, KeyCode.Backspace },
            { Key.Tab, KeyCode.Tab },
            { Key.Enter, KeyCode.Return },
            { Key.Space, KeyCode.Space },
            { Key.Comma, KeyCode.Comma },
            { Key.Minus, KeyCode.Minus },
            { Key.Period, KeyCode.Period },
            { Key.Slash, KeyCode.Slash },
            { Key.Digit0, KeyCode.Alpha0 },
            { Key.Digit1, KeyCode.Alpha1 },
            { Key.Digit2, KeyCode.Alpha2 },
            { Key.Digit3, KeyCode.Alpha3 },
            { Key.Digit4, KeyCode.Alpha4 },
            { Key.Digit5, KeyCode.Alpha5 },
            { Key.Digit6, KeyCode.Alpha6 },
            { Key.Digit7, KeyCode.Alpha7 },
            { Key.Digit8, KeyCode.Alpha8 },
            { Key.Digit9, KeyCode.Alpha9 },
            { Key.Semicolon, KeyCode.Semicolon },
            { Key.Equals, KeyCode.Equals },
            { Key.LeftBracket, KeyCode.LeftBracket },
            { Key.Backslash, KeyCode.Backslash },
            { Key.RightBracket, KeyCode.RightBracket },
            { Key.Backquote, KeyCode.BackQuote },
            { Key.Quote, KeyCode.Quote },
            { Key.A, KeyCode.A },
            { Key.B, KeyCode.B },
            { Key.C, KeyCode.C },
            { Key.D, KeyCode.D },
            { Key.E, KeyCode.E },
            { Key.F, KeyCode.F },
            { Key.G, KeyCode.G },
            { Key.H, KeyCode.H },
            { Key.I, KeyCode.I },
            { Key.J, KeyCode.J },
            { Key.K, KeyCode.K },
            { Key.L, KeyCode.L },
            { Key.M, KeyCode.M },
            { Key.N, KeyCode.N },
            { Key.O, KeyCode.O },
            { Key.P, KeyCode.P },
            { Key.Q, KeyCode.Q },
            { Key.R, KeyCode.R },
            { Key.S, KeyCode.S },
            { Key.T, KeyCode.T },
            { Key.U, KeyCode.U },
            { Key.V, KeyCode.V },
            { Key.W, KeyCode.W },
            { Key.X, KeyCode.X },
            { Key.Y, KeyCode.Y },
            { Key.Z, KeyCode.Z },
            { Key.F1, KeyCode.F1 },
            { Key.F2, KeyCode.F2 },
            { Key.F3, KeyCode.F3 },
            { Key.F4, KeyCode.F4 },
            { Key.F5, KeyCode.F5 },
            { Key.F6, KeyCode.F6 },
            { Key.F7, KeyCode.F7 },
            { Key.F8, KeyCode.F8 },
            { Key.F9, KeyCode.F9 },
            { Key.F10, KeyCode.F10 },
            { Key.F11, KeyCode.F11 },
            { Key.F12, KeyCode.F12 },
            { Key.None, KeyCode.None },
            { Key.LeftArrow, KeyCode.LeftArrow },
            { Key.RightArrow, KeyCode.RightArrow },
            { Key.UpArrow, KeyCode.UpArrow },
            { Key.DownArrow, KeyCode.DownArrow },
            { Key.LeftShift, KeyCode.LeftShift },
            { Key.RightShift, KeyCode.RightShift },
            { Key.Delete, KeyCode.Delete },
            { Key.Escape, KeyCode.Escape },
            { Key.LeftAlt, KeyCode.LeftAlt },
            { Key.RightAlt, KeyCode.RightAlt },
            { Key.LeftApple, KeyCode.LeftApple },
            { Key.RightApple, KeyCode.RightApple }
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
                if(control is KeyControl keyControl)
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
                    else if(s_KeyMap.TryGetValue(key, out var value))
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
            var obj = UnityEngine.EventSystems.EventSystem.current.currentSelectedGameObject;
            if (obj == null)
                return;

            var field = FindInputField(obj);
            if (field == null)
                return;
            Event e = CreateEvent(ptr);
            if(e != null)
            {
                field.ProcessEvent(e);
                field.ForceLabelUpdate();
            }
        }
    }
}
