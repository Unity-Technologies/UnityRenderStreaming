using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using UnityEngine.InputSystem;
using UnityEngine.EventSystems;

namespace Unity.RenderStreaming
{
    public class UIController : MonoBehaviour
    {
#pragma warning disable 0649
        [SerializeField] RectTransform cursor;
        [SerializeField] Text text;

#pragma warning restore 0649

        Image image;
        Vector2 baseSize;
        float scroll = 1f;
        float multiplier = 0.01f;

        void Start()
        {
            image = cursor.GetComponent<Image>();
            baseSize = cursor.sizeDelta;

            RemoteInput.Initialize();
            Keyboard.current.onTextInput += OnTextInput;
        }

        void OnTextInput(char c)
        {
            text.text += c;
        }

        void FixedUpdate()
        {
            var delta = Mouse.current.delta.ReadValue();
            cursor.Translate(delta.x, -delta.y, 0);

            if (Touchscreen.current.activeTouches.Count > 0)
            {
                cursor.position = Touchscreen.current.activeTouches[0].position.ReadValue();
            }

            if (!Keyboard.current.anyKey.isPressed)
            {
                text.text = string.Empty;
            }


            if (Mouse.current.leftButton.ReadValue() > 0)
            {
                image.color = Color.red;
            }
            else if (Mouse.current.middleButton.ReadValue() > 0)
            {
                image.color = Color.blue;
            }
            else if (Mouse.current.rightButton.ReadValue() > 0)
            {
                image.color = Color.green;
            }
            else
            {
                image.color = Color.white;
            }
            if(!Mathf.Approximately(Mouse.current.scroll.ReadValue().y, 0f))
            {
                scroll += Mouse.current.scroll.ReadValue().y * multiplier;
                cursor.sizeDelta = baseSize * scroll;
            }
        }
    }

}
