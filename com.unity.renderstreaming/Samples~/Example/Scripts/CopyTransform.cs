using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

class CopyTransform : MonoBehaviour
{
    [SerializeField] private Transform origin;

    void Update()
    {
        transform.position = origin.position;
        transform.rotation = origin.rotation;
    }
}
