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
#pragma warning restore 0649

        Image image;

        Mouse mouse;
        Keyboard keyboard;
        Touchscreen touch;
        void Start()
        {
            image = cursor.GetComponent<Image>();
            mouse = RemoteInput.Mouse;
            keyboard = RemoteInput.Keyboard;
            touch = RemoteInput.Touch;
        }

        void FixedUpdate()
        {
            var delta = mouse.delta.ReadValue();
            cursor.Translate(delta.x, -delta.y, 0);

            if (touch.activeTouches.Count > 0)
            {
                cursor.position = touch.activeTouches[0].position.ReadValue();
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
        }
    }

}
