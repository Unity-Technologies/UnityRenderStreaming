using UnityEngine;
using UnityEngine.UI;
using UnityEngine.InputSystem.Layouts;
using UnityEngine.InputSystem.XR;

namespace Unity.RenderStreaming.Samples
{
    using InputSystem = UnityEngine.InputSystem.InputSystem;

    static class InputReceiverExtension
    {
        public static void SetInputRange(this InputReceiver reveiver, Vector2Int size)
        {
            reveiver.SetInputRange(size, new Rect(0, 0, Screen.width, Screen.height));
        }
    }

    class BroadcastSample : MonoBehaviour
    {
        [SerializeField] RenderStreaming renderStreaming;
        [SerializeField] InputReceiver inputReceiver;
        [SerializeField] VideoStreamSender videoStreamSender;

        private void Awake()
        {
#if URS_USE_AR_FOUNDATION
            InputSystem.RegisterLayout<UnityEngine.XR.ARSubsystems.HandheldARInputDevice>(
                matches: new InputDeviceMatcher()
                    .WithInterface(XRUtilities.InterfaceMatchAnyVersion)
                );
#endif

            if (videoStreamSender.streamingSize != RenderStreamingSettings.StreamSize)
            {
                videoStreamSender.streamingSize = RenderStreamingSettings.StreamSize;
            }
            videoStreamSender.FilterVideoCodecs(RenderStreamingSettings.SelectSenderVideoCodecIndex);
        }

        void Start()
        {
            if (renderStreaming.runOnAwake)
                return;
            renderStreaming.Run(signaling: RenderStreamingSettings.Signaling);

            inputReceiver.OnStartedChannel += OnStartedChannel;
        }

        void OnStartedChannel(string connectionId)
        {
            inputReceiver.SetInputRange(videoStreamSender.streamingSize);
            inputReceiver.SetEnableInputPositionCorrection(true);
        }
    }
}
