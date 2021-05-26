using UnityEngine;

namespace Unity.RenderStreaming.Samples
{
    class BroadcastSample : MonoBehaviour
    {
        [SerializeField] RenderStreaming renderStreaming;

        // Start is called before the first frame update
        void Start()
        {
            if (renderStreaming.runOnAwake)
                return;
            renderStreaming.Run(
                hardwareEncoder: RenderStreamingSettings.EnableHWCodec,
                signaling: RenderStreamingSettings.Signaling);

        }
    }
}
