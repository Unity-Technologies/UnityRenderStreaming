using System;
using System.Collections.Generic;
using System.Linq;
using UnityEngine;
using UnityEngine.InputSystem;
using UnityEngine.InputSystem.Layouts;
using UnityEngine.InputSystem.XR;
using UnityEngine.UI;

namespace Unity.RenderStreaming.Samples
{
    using InputSystem = UnityEngine.InputSystem.InputSystem;

    static class InputReceiverExtension
    {
        public static void CalculateInputRegion(this InputReceiver reveiver, Vector2Int size)
        {
            reveiver.CalculateInputRegion(size, new Rect(0, 0, Screen.width, Screen.height));
        }
    }

    static class InputActionExtension
    {
        public static void AddListener(this InputAction action, Action<InputAction.CallbackContext> callback)
        {
            action.started += callback;
            action.performed += callback;
            action.canceled += callback;
        }
    }

    class BroadcastSample : MonoBehaviour
    {
        [SerializeField] private SignalingManager renderStreaming;
        [SerializeField] private InputReceiver inputReceiver;
        [SerializeField] private SimpleCameraControllerV2 cameraController;
        [SerializeField] private UIControllerV2 uiController;
        [SerializeField] private VideoStreamSender videoStreamSender;
        [SerializeField] private Dropdown videoSourceTypeSelector;
        [SerializeField] private Dropdown bandwidthSelector;
        [SerializeField] private Dropdown scaleResolutionDownSelector;
        [SerializeField] private Dropdown framerateSelector;
        [SerializeField] private Dropdown resolutionSelector;

        private Dictionary<string, VideoStreamSource> videoSourceTypeOptions = new Dictionary<string, VideoStreamSource>
        {
            {"Screen", VideoStreamSource.Screen },
            {"Camera", VideoStreamSource.Camera },
            {"Texture", VideoStreamSource.Texture },
            {"WebCam", VideoStreamSource.WebCamera }
        };

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
        private int lastWidth;
        private int lastHeight;

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
            videoSourceTypeSelector.options = videoSourceTypeOptions
                .Select(pair => new Dropdown.OptionData { text = pair.Key })
                .ToList();
            videoSourceTypeSelector.onValueChanged.AddListener(ChangeVideoSourceType);

            bandwidthSelector.options = bandwidthOptions
                .Select(pair => new Dropdown.OptionData { text = pair.Key })
                .ToList();
            bandwidthSelector.options.Add(new Dropdown.OptionData { text = "Custom" });
            bandwidthSelector.onValueChanged.AddListener(ChangeBandwidth);

            scaleResolutionDownSelector.options = scaleResolutionDownOptions
                .Select(pair => new Dropdown.OptionData { text = pair.Key })
                .ToList();
            scaleResolutionDownSelector.options.Add(new Dropdown.OptionData { text = "Custom" });
            scaleResolutionDownSelector.onValueChanged.AddListener(ChangeScaleResolutionDown);

            framerateSelector.options = framerateOptions
                .Select(pair => new Dropdown.OptionData { text = pair.Key })
                .ToList();
            framerateSelector.options.Add(new Dropdown.OptionData { text = "Custom" });
            framerateSelector.onValueChanged.AddListener(ChangeFramerate);

            resolutionSelector.options = resolutionOptions
                .Select(pair => new Dropdown.OptionData { text = pair.Key })
                .ToList();
            resolutionSelector.options.Add(new Dropdown.OptionData { text = "Custom" });
            resolutionSelector.onValueChanged.AddListener(ChangeResolution);
        }

        private void ChangeVideoSourceType(int index)
        {
            var source = videoSourceTypeOptions.Values.ElementAt(index);
            videoStreamSender.source = source;
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
            CalculateInputRegion();
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
            CalculateInputRegion();
        }

        private void Start()
        {
            SyncDisplayVideoSenderParameters();

            if (renderStreaming.runOnAwake)
                return;
            if (settings != null)
                renderStreaming.useDefaultSettings = settings.UseDefaultSettings;
            if (settings?.SignalingSettings != null)
                renderStreaming.SetSignalingSettings(settings.SignalingSettings);
            renderStreaming.Run();

            inputReceiver.OnStartedChannel += OnStartedChannel;
            var map = inputReceiver.currentActionMap;
            map["Movement"].AddListener(cameraController.OnMovement);
            map["Look"].AddListener(cameraController.OnLook);
            map["ResetCamera"].AddListener(cameraController.OnResetCamera);
            map["Rotate"].AddListener(cameraController.OnRotate);
            map["Position"].AddListener(cameraController.OnPosition);
            map["Point"].AddListener(uiController.OnPoint);
            map["Press"].AddListener(uiController.OnPress);
            map["PressAnyKey"].AddListener(uiController.OnPressAnyKey);
        }

        private void OnStartedChannel(string connectionId)
        {
            CalculateInputRegion();
        }

        // Parameters can be changed from the Unity Editor inspector when in Play Mode,
        // So this method monitors the parameters every frame and updates scene UI.
        private void Update()
        {
#if UNITY_EDITOR
            SyncDisplayVideoSenderParameters();
#endif
            // Call SetInputChange if window size is changed.
            var width = Screen.width;
            var height = Screen.height;
            if (lastWidth == width && lastHeight == height)
                return;
            lastWidth = width;
            lastHeight = height;

            CalculateInputRegion();
        }

        private void CalculateInputRegion()
        {
            if (!inputReceiver.IsConnected)
                return;
            var width = (int)(videoStreamSender.width / videoStreamSender.scaleResolutionDown);
            var height = (int)(videoStreamSender.height / videoStreamSender.scaleResolutionDown);
            inputReceiver.CalculateInputRegion(new Vector2Int(width, height));
            inputReceiver.SetEnableInputPositionCorrection(true);
        }

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

            var target = new Vector2Int((int)videoStreamSender.width, (int)videoStreamSender.height);
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
