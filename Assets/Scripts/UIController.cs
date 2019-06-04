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
        [SerializeField] RectTransform cursor;
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

            var touch0 = touch.primaryTouch.ReadValue();
            if (touch0.pressure > 0)
            {
                cursor.position = touch0.position;
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
