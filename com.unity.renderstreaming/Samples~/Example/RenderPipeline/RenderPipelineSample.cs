using UnityEngine;

namespace Unity.RenderStreaming.Samples
{
    class RenderPipelineSample : MonoBehaviour
    {
        [SerializeField] RenderStreaming renderStreaming;
        [SerializeField] VideoStreamSender videoStreamSender;

        private void Awake()
        {
            if (videoStreamSender.streamingSize != RenderStreamingSettings.StreamSize)
            {
                videoStreamSender.streamingSize = RenderStreamingSettings.StreamSize;
            }
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
