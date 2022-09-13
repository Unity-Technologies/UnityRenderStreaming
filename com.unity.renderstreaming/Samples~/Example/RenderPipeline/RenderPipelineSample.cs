using System.Collections.Generic;
using System.Linq;
using UnityEngine;
using UnityEngine.UI;

namespace Unity.RenderStreaming.Samples
{
    class RenderPipelineSample : MonoBehaviour
    {
        [SerializeField] RenderStreaming renderStreaming;
        [SerializeField] VideoStreamSender videoStreamSender;
        [SerializeField] private Dropdown bandwidthSelector;
        [SerializeField] private Dropdown scaleResolutionDownSelector;
        [SerializeField] private Dropdown framerateSelector;
        [SerializeField] private Dropdown resolutionSelector;

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

        private Dictionary<string, Vector2Int> resolutionOptions =
            new Dictionary<string, Vector2Int>
            {
                { "640x480", new Vector2Int(640, 480) },
                { "1280x720", new Vector2Int(1280, 720) },
                { "1600x1200", new Vector2Int(1600, 1200) },
                { "1920x1200", new Vector2Int(1920, 1200) },
                { "2560x1440", new Vector2Int(2560, 1440) },
            };

        private RenderStreamingSettings settings;

        private void Awake()
        {
            settings = SampleManager.Instance.Settings;

            if(settings != null)
            {
                videoStreamSender.width = (uint)settings.StreamSize.x;
                videoStreamSender.height = (uint)settings.StreamSize.y;
                videoStreamSender.SetCodec(settings.SenderVideoCodec);
            }

            bandwidthSelector.options = bandwidthOptions
                .Select(pair => new Dropdown.OptionData { text = pair.Key })
                .ToList();
            framerateSelector.SetValueWithoutNotify(2); // todo: detect default select index
            bandwidthSelector.onValueChanged.AddListener(ChangeBandwidth);
            scaleResolutionDownSelector.options = scaleResolutionDownOptions
                .Select(pair => new Dropdown.OptionData { text = pair.Key })
                .ToList();
            scaleResolutionDownSelector.onValueChanged.AddListener(ChangeScaleResolutionDown);

            framerateSelector.options = framerateOptions
                .Select(pair => new Dropdown.OptionData { text = pair.Key })
                .ToList();
            framerateSelector.SetValueWithoutNotify(2); // todo: detect default select index
            framerateSelector.onValueChanged.AddListener(ChangeFramerate);

            resolutionSelector.options = resolutionOptions
                .Select(pair => new Dropdown.OptionData { text = pair.Key })
                .ToList();
            resolutionSelector.SetValueWithoutNotify(1); // todo: detect default select index
            resolutionSelector.onValueChanged.AddListener(ChangeResolution);
        }

        private void ChangeBandwidth(int index)
        {
            var bitrate = bandwidthOptions.Values.ElementAt(index);
            videoStreamSender.SetBitrate(bitrate, bitrate);
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

        private void ChangeResolution(int index)
        {
            var resolution = resolutionOptions.Values.ElementAt(index);

            videoStreamSender.SetTextureSize(resolution);
        }

        private void Start()
        {
            if (!renderStreaming.runOnAwake)
            {
                renderStreaming.Run(signaling: settings?.Signaling);
            }
        }
    }
}
