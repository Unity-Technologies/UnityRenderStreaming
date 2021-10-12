using System.Linq;
using System.Collections.Generic;
using UnityEngine;

namespace Unity.RenderStreaming.Samples
{
    public class FollowTransform : MonoBehaviour
    {
        [SerializeField] Transform target;
        [SerializeField] Vector3 offset;

        [SerializeField] bool followPosition;
        [SerializeField] bool followRotation;

        private void Update()
        {
            if (followPosition)
                transform.localPosition = target.localPosition + offset;
            if(followRotation)
                transform.localRotation = target.localRotation;
        }
    }
}
