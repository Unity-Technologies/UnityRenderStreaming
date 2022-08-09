using System.Collections.Generic;
using System.Linq;
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
        [SerializeField] private RenderStreaming renderStreaming;
        [SerializeField] private InputReceiver inputReceiver;
        [SerializeField] private VideoStreamSender videoStreamSender;
        [SerializeField] private Dropdown bandwidthSelector;
        [SerializeField] private Dropdown scaleResolutionDownSelector;
        [SerializeField] private Dropdown framerateSelector;

        private Dictionary<string, uint> bandwidthOptions =
            new Dictionary<string, uint>()
            {
                { "10000", 10000 },
                { "2000", 2000 },
                { "1000", 1000 },
                { "500",  500 },
                { "250",  250 },
                { "125",  125 },
            };

        private Dictionary<string, float> scaleResolutionDownOptions =
            new Dictionary<string, float>()
            {
                { "Not scaling", 1.0f },
                { "Down scale by 2.0", 2.0f },
                { "Down scale by 4.0", 4.0f },
                { "Down scale by 8.0", 8.0f },
                { "Down scale by 16.0", 16.0f }
            };

        private Dictionary<string, float> framerateOptions =
            new Dictionary<string, float>
            {
                { "90", 90f },
                { "60", 60f },
                { "30", 30f },
                { "20", 20f },
                { "10", 10f },
                { "5", 5f },
            };

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

            bandwidthSelector.options = bandwidthOptions
                .Select(pair => new Dropdown.OptionData {text = pair.Key})
                .ToList();
            framerateSelector.SetValueWithoutNotify(2); // todo: detect default select index
            bandwidthSelector.onValueChanged.AddListener(ChangeBandwidth);
            scaleResolutionDownSelector.options = scaleResolutionDownOptions
                .Select(pair => new Dropdown.OptionData {text = pair.Key})
                .ToList();
            scaleResolutionDownSelector.onValueChanged.AddListener(ChangeScaleResolutionDown);

            framerateSelector.options = framerateOptions
                .Select(pair => new Dropdown.OptionData {text = pair.Key})
                .ToList();
            framerateSelector.SetValueWithoutNotify(2); // todo: detect default select index
            framerateSelector.onValueChanged.AddListener(ChangeFramerate);
        }

        private void ChangeBandwidth(int index)
        {
            var bandwidth = bandwidthOptions.Values.ElementAt(index);
            videoStreamSender.SetBitrate(bandwidth);
        }

        private void ChangeScaleResolutionDown(int index)
        {
            var scale = scaleResolutionDownOptions.Values.ElementAt(index);
            videoStreamSender.SetScaleResolutionDown(scale);
        }

        private void ChangeFramerate(int index)
        {
            var framerate = framerateOptions.Values.ElementAt(index);
            videoStreamSender.SetFrameRate(framerate);
        }

        private void Start()
        {
            if (renderStreaming.runOnAwake)
                return;
            renderStreaming.Run(signaling: RenderStreamingSettings.Signaling);

            inputReceiver.OnStartedChannel += OnStartedChannel;
        }

        private void OnStartedChannel(string connectionId)
        {
            inputReceiver.SetInputRange(videoStreamSender.streamingSize);
            inputReceiver.SetEnableInputPositionCorrection(true);
        }
    }
}
