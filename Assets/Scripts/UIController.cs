using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using UnityEngine.InputSystem;
using UnityEngine.EventSystems;

public class UIController : MonoBehaviour
{
    [SerializeField] RectTransform cursor;
    Image image;
    void Start()
    {
        image = cursor.GetComponent<Image>();
    }

    void FixedUpdate()
    {
        var delta = Mouse.current.delta.ReadValue();
        cursor.Translate(delta.x, -delta.y, 0);

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
