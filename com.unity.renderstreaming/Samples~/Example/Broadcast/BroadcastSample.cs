using System.Collections;
using System.Collections.Generic;
using UnityEngine;

namespace Unity.RenderStreaming.Samples
{
    public class BroadcastSample : MonoBehaviour
    {
        [SerializeField] RenderStreaming renderStreaming;

        // Start is called before the first frame update
        void Start()
        {
            renderStreaming.Run(
                hardwareEncoder: RenderStreamingSettings.EnableHWCodec,
                signaling:RenderStreamingSettings.Signaling);
        }
    }
}
