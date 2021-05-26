using UnityEngine;

class CopyTransform : MonoBehaviour
{
    [SerializeField] private Transform origin;

    void Update()
    {
        transform.position = origin.position;
        transform.rotation = origin.rotation;
    }
}
