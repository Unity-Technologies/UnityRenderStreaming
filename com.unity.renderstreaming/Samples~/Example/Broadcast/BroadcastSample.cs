using System;
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
#if URS_USE_AR_FOUNDATION
            InputSystem.RegisterLayout<UnityEngine.XR.ARSubsystems.HandheldARInputDevice>(
                matches: new InputDeviceMatcher()
                    .WithInterface(XRUtilities.InterfaceMatchAnyVersion)
            );
#endif
            settings = SampleManager.Instance.Settings;
            if (settings != null)
            {
                if (videoStreamSender.source != VideoStreamSource.Texture)
                {
                    videoStreamSender.width = (uint)settings.StreamSize.x;
                    videoStreamSender.height = (uint)settings.StreamSize.y;
                }
                videoStreamSender.SetCodec(settings.SenderVideoCodec);
            }

            bandwidthSelector.options = bandwidthOptions
                .Select(pair => new Dropdown.OptionData {text = pair.Key})
                .ToList();
            bandwidthSelector.options.Add(new Dropdown.OptionData {text = "Custom"});
            bandwidthSelector.onValueChanged.AddListener(ChangeBandwidth);

            scaleResolutionDownSelector.options = scaleResolutionDownOptions
                .Select(pair => new Dropdown.OptionData {text = pair.Key})
                .ToList();
            scaleResolutionDownSelector.options.Add(new Dropdown.OptionData {text = "Custom"});
            scaleResolutionDownSelector.onValueChanged.AddListener(ChangeScaleResolutionDown);

            framerateSelector.options = framerateOptions
                .Select(pair => new Dropdown.OptionData {text = pair.Key})
                .ToList();
            framerateSelector.options.Add(new Dropdown.OptionData {text = "Custom"});
            framerateSelector.onValueChanged.AddListener(ChangeFramerate);

            resolutionSelector.options = resolutionOptions
                .Select(pair => new Dropdown.OptionData {text = pair.Key})
                .ToList();
            resolutionSelector.options.Add(new Dropdown.OptionData {text = "Custom"});
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

            if (videoStreamSender.source != VideoStreamSource.Texture)
                videoStreamSender.SetTextureSize(resolution);
        }

        private void Start()
        {
            SyncDisplayVideoSenderParameters();

            if (renderStreaming.runOnAwake)
                return;
            renderStreaming.Run(signaling: settings?.Signaling);

            inputReceiver.OnStartedChannel += OnStartedChannel;
        }

        private void OnStartedChannel(string connectionId)
        {
            inputReceiver.SetInputRange(new Vector2Int((int)videoStreamSender.width, (int)videoStreamSender.height));
            inputReceiver.SetEnableInputPositionCorrection(true);
        }

        // Parameters can be changed from the Unity Editor inspector when in Play Mode,
        // So this method monitors the parameters every frame and updates scene UI.
#if UNITY_EDITOR
        private void Update()
        {
            SyncDisplayVideoSenderParameters();
        }
#endif

        private void SyncDisplayVideoSenderParameters()
        {
            if (videoStreamSender == null)
            {
                return;
            }


            var bandwidthIndex = Array.IndexOf(bandwidthOptions.Values.ToArray(), videoStreamSender.maxBitrate);
            if (bandwidthIndex < 0)
            {
                bandwidthSelector.SetValueWithoutNotify(bandwidthSelector.options.Count - 1);
            }
            else
            {
                bandwidthSelector.SetValueWithoutNotify(bandwidthIndex);
            }

            var scaleDownIndex = Array.IndexOf(scaleResolutionDownOptions.Values.ToArray(), videoStreamSender.scaleResolutionDown);
            if (scaleDownIndex < 0)
            {
                scaleResolutionDownSelector.SetValueWithoutNotify(bandwidthSelector.options.Count - 1);
            }
            else
            {
                scaleResolutionDownSelector.SetValueWithoutNotify(scaleDownIndex);
            }

            var framerateIndex = Array.IndexOf(framerateOptions.Values.ToArray(), videoStreamSender.frameRate);
            if (framerateIndex < 0)
            {
                framerateSelector.SetValueWithoutNotify(framerateSelector.options.Count - 1);
            }
            else
            {
                framerateSelector.SetValueWithoutNotify(framerateIndex);
            }

            var target = new Vector2Int((int) videoStreamSender.width, (int) videoStreamSender.height);
            var resolutionIndex = Array.IndexOf(resolutionOptions.Values.ToArray(), target);
            if (resolutionIndex < 0)
            {
                resolutionSelector.SetValueWithoutNotify(resolutionSelector.options.Count - 1);
            }
            else
            {
                resolutionSelector.SetValueWithoutNotify(resolutionIndex);
            }
        }
    }
}
