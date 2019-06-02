using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.InputSystem;
using UnityEngine.EventSystems;

public class UIController : MonoBehaviour
{
    [SerializeField] RectTransform cursor;

    void FixedUpdate()
    {
        var delta = Mouse.current.delta.ReadValue();
        cursor.Translate(delta.x, -delta.y, 0);
    }
}
