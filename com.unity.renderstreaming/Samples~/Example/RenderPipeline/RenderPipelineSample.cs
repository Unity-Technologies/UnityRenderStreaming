using UnityEngine;

namespace Unity.RenderStreaming.Samples
{
    class RenderPipelineSample : MonoBehaviour
    {
        [SerializeField] RenderStreaming renderStreaming;
        [SerializeField] VideoStreamSender videoStreamSender;

        private void Awake()
        {
            videoStreamSender.width = (uint)RenderStreamingSettings.StreamSize.x;
            videoStreamSender.height = (uint)RenderStreamingSettings.StreamSize.y;
        }

        private void Start()
        {
            if (!renderStreaming.runOnAwake)
            {
                renderStreaming.Run(signaling: RenderStreamingSettings.Signaling);
            }
        }
    }
}
