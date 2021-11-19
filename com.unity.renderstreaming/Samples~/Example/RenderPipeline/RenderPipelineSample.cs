using UnityEngine;

namespace Unity.RenderStreaming.Samples
{
    class RenderPipelineSample : MonoBehaviour
    {
        [SerializeField] RenderStreaming renderStreaming;

        // Start is called before the first frame update
        void Start()
        {
            if (!renderStreaming.runOnAwake)
            {
                renderStreaming.Run(
                    hardwareEncoder: RenderStreamingSettings.EnableHWCodec,
                    signaling: RenderStreamingSettings.Signaling);
            }
        }
    }
}
