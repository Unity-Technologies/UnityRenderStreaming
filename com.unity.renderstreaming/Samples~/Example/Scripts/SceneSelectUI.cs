using System;
using System.Collections;
using System.Collections.Generic;
using System.Globalization;
using System.Linq;
using UnityEngine;
using UnityEngine.SceneManagement;
using UnityEngine.UI;
using Gyroscope = UnityEngine.InputSystem.Gyroscope;

#if URS_USE_AR_FOUNDATION
using UnityEngine.XR.ARFoundation;
#endif

namespace Unity.RenderStreaming.Samples
{
    internal enum SignalingType
    {
        WebSocket,
        Http,
    }

    internal class RenderStreamingSettings
    {
        public const int DefaultStreamWidth = 1280;
        public const int DefaultStreamHeight = 720;

        private bool useDefaultSettings = true;
        private SignalingType signalingType = SignalingType.WebSocket;
        private string signalingAddress = "localhost";
        private int signalingInterval = 5000;
        private bool signalingSecured = false;
        private Vector2Int streamSize = new Vector2Int(DefaultStreamWidth, DefaultStreamHeight);
        private VideoCodecInfo receiverVideoCodec = null;
        private VideoCodecInfo senderVideoCodec = null;

        public bool UseDefaultSettings
        {
            get { return useDefaultSettings; }
            set { useDefaultSettings = value; }
        }

        public SignalingType SignalingType
        {
            get { return signalingType; }
            set { signalingType = value; }
        }

        public string SignalingAddress
        {
            get { return signalingAddress; }
            set { signalingAddress = value; }
        }

        public bool SignalingSecured
        {
            get { return signalingSecured; }
            set { signalingSecured = value; }
        }

        public int SignalingInterval
        {
            get { return signalingInterval; }
            set { signalingInterval = value; }
        }

        public SignalingSettings SignalingSettings
        {
            get
            {
                switch (signalingType)
                {
                    case SignalingType.WebSocket:
                        {
                            var schema = signalingSecured ? "wss" : "ws";
                            return new WebSocketSignalingSettings
                            (
                                url: $"{schema}://{signalingAddress}"
                            );
                        }
                    case SignalingType.Http:
                        {
                            var schema = signalingSecured ? "https" : "http";
                            return new HttpSignalingSettings
                            (
                                url: $"{schema}://{signalingAddress}",
                                interval: signalingInterval
                            );
                        }
                }
                throw new InvalidOperationException();
            }
        }

        public Vector2Int StreamSize
        {
            get { return streamSize; }
            set { streamSize = value; }
        }

        public VideoCodecInfo ReceiverVideoCodec
        {
            get { return receiverVideoCodec; }
            set { receiverVideoCodec = value; }
        }

        public VideoCodecInfo SenderVideoCodec
        {
            get { return senderVideoCodec; }
            set { senderVideoCodec = value; }
        }
    }

    internal class SceneSelectUI : MonoBehaviour
    {
        [SerializeField] private Toggle toggleUseDefaultSettings;
        [SerializeField] private Dropdown dropdownSignalingType;
        [SerializeField] private InputField inputFieldSignalingAddress;
        [SerializeField] private Toggle toggleSignalingSecured;
        [SerializeField] private InputField inputFieldSignalingInterval;
        [SerializeField] private Dropdown streamSizeSelector;
        [SerializeField] private InputField textureWidthInput;
        [SerializeField] private InputField textureHeightInput;
        [SerializeField] private Dropdown receiverVideoCodecSelector;
        [SerializeField] private Dropdown senderVideoCodecSelector;

        [SerializeField] private Button buttonBidirectional;
        [SerializeField] private Button buttonBroadcast;
        [SerializeField] private Button buttonGyro;
        [SerializeField] private Button buttonRenderPipeline;
        [SerializeField] private Button buttonReceiver;
        [SerializeField] private Button buttonWebBrowserInput;
        [SerializeField] private Button buttonAR;
        [SerializeField] private Button buttonMultiplay;

        [SerializeField]
        private List<Vector2Int> streamSizeList = new List<Vector2Int>
        {
            new Vector2Int(640, 360),
            new Vector2Int(1280, 720),
            new Vector2Int(1920, 1080),
            new Vector2Int(2560, 1440),
            new Vector2Int(3840, 2160),
            new Vector2Int(360, 640),
            new Vector2Int(720, 1280),
            new Vector2Int(1080, 1920),
            new Vector2Int(1440, 2560),
            new Vector2Int(2160, 3840),
        };

        private RenderStreamingSettings settings;

        static string CodecTitle(VideoCodecInfo codec)
        {
            switch (codec)
            {
                case H264CodecInfo h264Codec:
                    return $"{h264Codec.mimeType} {h264Codec.profile} {h264Codec.level.ToString().Insert(1, ".")} {h264Codec.codecImplementation}";
                case VP9CodecInfo V9Codec:
                    return $"{V9Codec.mimeType} {V9Codec.profile} {V9Codec.codecImplementation}";
                case AV1CodecInfo av1Codec:
                    return $"{av1Codec.mimeType} {av1Codec.profile} {av1Codec.codecImplementation}";
                default:
                    return $"{codec.mimeType} {codec.codecImplementation}";
            }
            throw new ArgumentException();
        }

        void Start()
        {
            SampleManager.Instance.Initialize();
            settings = SampleManager.Instance.Settings;

            toggleUseDefaultSettings.isOn = settings.UseDefaultSettings;
            dropdownSignalingType.value = (int)settings.SignalingType;
            inputFieldSignalingAddress.text = settings.SignalingAddress;
            toggleSignalingSecured.isOn = settings.SignalingSecured;
            inputFieldSignalingInterval.text =
                settings.SignalingInterval.ToString(CultureInfo.InvariantCulture);

            toggleUseDefaultSettings.onValueChanged.AddListener(OnChangeUseDefaultSettings);
            dropdownSignalingType.onValueChanged.AddListener(OnChangeSignalingType);
            inputFieldSignalingAddress.onValueChanged.AddListener(OnChangeSignalingAddress);
            toggleSignalingSecured.onValueChanged.AddListener(OnChangeSignalingSecured);
            inputFieldSignalingInterval.onValueChanged.AddListener(OnChangeSignalingInterval);

            SetInteractableSignalingUI(!settings.UseDefaultSettings);

            var optionList = streamSizeList.Select(size => new Dropdown.OptionData($" {size.x} x {size.y} ")).ToList();
            optionList.Add(new Dropdown.OptionData(" Custom "));
            streamSizeSelector.options = optionList;

            var existInList = streamSizeList.Contains(settings.StreamSize);
            if (existInList)
            {
                streamSizeSelector.value = streamSizeList.IndexOf(settings.StreamSize);
            }
            else
            {
                streamSizeSelector.value = optionList.Count - 1;
                textureWidthInput.text = settings.StreamSize.x.ToString();
                textureHeightInput.text = settings.StreamSize.y.ToString();
                textureWidthInput.interactable = true;
                textureHeightInput.interactable = true;
            }

            streamSizeSelector.onValueChanged.AddListener(OnChangeStreamSizeSelect);
            textureWidthInput.onValueChanged.AddListener(OnChangeTextureWidthInput);
            textureHeightInput.onValueChanged.AddListener(OnChangeTextureHeightInput);

            var receiverVideoCodecList = VideoStreamReceiver.GetAvailableCodecs()
                .Select(codec => new Dropdown.OptionData(CodecTitle(codec))).ToList();
            receiverVideoCodecSelector.options.AddRange(receiverVideoCodecList);

            var senderVideoCodecList = VideoStreamSender.GetAvailableCodecs()
                .Select(codec => new Dropdown.OptionData(CodecTitle(codec))).ToList();
            senderVideoCodecSelector.options.AddRange(senderVideoCodecList);

            receiverVideoCodecSelector.value = Array.FindIndex(VideoStreamReceiver.GetAvailableCodecs().ToArray(), codec => codec.Equals(settings.ReceiverVideoCodec)) + 1;
            senderVideoCodecSelector.value = Array.FindIndex(VideoStreamSender.GetAvailableCodecs().ToArray(), codec => codec.Equals(settings.SenderVideoCodec)) + 1;

            receiverVideoCodecSelector.onValueChanged.AddListener(OnChangeReceiverVideoCodecSelect);
            senderVideoCodecSelector.onValueChanged.AddListener(OnChangeSenderVideoCodecSelect);
            buttonBidirectional.onClick.AddListener(OnPressedBidirectional);
            buttonBroadcast.onClick.AddListener(OnPressedBroadcast);
            buttonGyro.onClick.AddListener(OnPressedGyro);
            buttonRenderPipeline.onClick.AddListener(OnPressedRenderPipeline);
            buttonReceiver.onClick.AddListener(OnPressedReceiver);
            buttonWebBrowserInput.onClick.AddListener(OnPressedWebBrowserInput);
            buttonAR.onClick.AddListener(OnPressedAR);
            buttonMultiplay.onClick.AddListener(OnPressedMultiplay);

#if UNITY_EDITOR
            buttonGyro.interactable = false;
#else
            // Gyro input is not supported on this device.
            if (Gyroscope.current == null)
            {
                buttonGyro.interactable = false;
            }
#endif


#if !URS_USE_URP_RUNTIME && !URS_USE_HDRP_RUNTIME
            buttonRenderPipeline.interactable = false;
#endif

#if URS_USE_AR_FOUNDATION
            StartCoroutine(CheckARAvailability(available => { buttonAR.interactable = available; }));
#else
            buttonAR.interactable = false;
#endif
        }


#if URS_USE_AR_FOUNDATION
        IEnumerator CheckARAvailability(Action<bool> callback)
        {
            if ((ARSession.state == ARSessionState.None) ||
                (ARSession.state == ARSessionState.CheckingAvailability))
            {
                yield return ARSession.CheckAvailability();
            }

            callback?.Invoke(ARSession.state == ARSessionState.Ready);
        }
#endif

        private void OnChangeSignalingType(int value)
        {
            settings.SignalingType =
                (SignalingType)Enum.GetValues(typeof(SignalingType)).GetValue(value);
        }

        private void OnChangeSignalingAddress(string value)
        {
            settings.SignalingAddress = value;
        }

        private void OnChangeSignalingSecured(bool value)
        {
            settings.SignalingSecured = value;
        }

        private void OnChangeUseDefaultSettings(bool value)
        {
            settings.UseDefaultSettings = value;
            SetInteractableSignalingUI(!value);
        }

        private void SetInteractableSignalingUI(bool interactable)
        {
            dropdownSignalingType.interactable = interactable;
            inputFieldSignalingAddress.interactable = interactable;
            toggleSignalingSecured.interactable = interactable;
            inputFieldSignalingInterval.interactable = interactable;
        }

        private void OnChangeSignalingInterval(string value)
        {
            if (int.TryParse(value, out int _value))
            {
                settings.SignalingInterval = _value;
            }
            else
            {
                settings.SignalingInterval = 5000;
            }
        }

        private void OnChangeStreamSizeSelect(int index)
        {
            var isCustom = index >= streamSizeList.Count;
            textureWidthInput.interactable = isCustom;
            textureHeightInput.interactable = isCustom;

            if (isCustom)
            {
                return;
            }

            settings.StreamSize = streamSizeList[index];
        }

        private void OnChangeTextureWidthInput(string input)
        {
            var height = settings.StreamSize.y;

            if (string.IsNullOrEmpty(input))
            {
                settings.StreamSize = new Vector2Int(RenderStreamingSettings.DefaultStreamWidth, height);
                return;
            }

            if (int.TryParse(input, out var width))
            {
                settings.StreamSize = new Vector2Int(width, height);
            }
        }

        private void OnChangeTextureHeightInput(string input)
        {
            var width = settings.StreamSize.x;

            if (string.IsNullOrEmpty(input))
            {
                settings.StreamSize = new Vector2Int(width, RenderStreamingSettings.DefaultStreamHeight);
                return;
            }

            if (int.TryParse(input, out var height))
            {
                settings.StreamSize = new Vector2Int(width, height);
            }
        }

        private void OnChangeSenderVideoCodecSelect(int index)
        {
            if (index == 0)
                settings.SenderVideoCodec = null;
            else
                settings.SenderVideoCodec = VideoStreamSender.GetAvailableCodecs().ElementAt(index - 1);
        }

        private void OnChangeReceiverVideoCodecSelect(int index)
        {
            if (index == 0)
                settings.ReceiverVideoCodec = null;
            else
                settings.ReceiverVideoCodec = VideoStreamReceiver.GetAvailableCodecs().ElementAt(index - 1);
        }

        private void OnPressedBidirectional()
        {
            SceneManager.LoadScene("Bidirectional", LoadSceneMode.Single);
        }

        private void OnPressedBroadcast()
        {
            SceneManager.LoadScene("Broadcast", LoadSceneMode.Single);
        }

        private void OnPressedGyro()
        {
            SceneManager.LoadScene("Gyro", LoadSceneMode.Single);
        }

        private void OnPressedRenderPipeline()
        {
#if URS_USE_URP_RUNTIME
            SceneManager.LoadScene("URP", LoadSceneMode.Single);
#elif URS_USE_HDRP_RUNTIME
            SceneManager.LoadScene("HDRP", LoadSceneMode.Single);
#else
            throw new InvalidOperationException("HDRP or URP package is not found in this project.");
#endif
        }

        private void OnPressedReceiver()
        {
            SceneManager.LoadScene("Receiver", LoadSceneMode.Single);
        }

        private void OnPressedWebBrowserInput()
        {
            SceneManager.LoadScene("WebBrowserInput", LoadSceneMode.Single);
        }

        private void OnPressedAR()
        {
            SceneManager.LoadScene("ARFoundation", LoadSceneMode.Single);
        }

        private void OnPressedMultiplay()
        {
            SceneManager.LoadScene("Multiplay", LoadSceneMode.Single);
        }
    }
}
