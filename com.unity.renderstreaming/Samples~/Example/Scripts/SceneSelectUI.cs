using System;
using System.Collections;
using System.Collections.Generic;
using System.Globalization;
using System.Linq;
using System.Threading;
using Unity.RenderStreaming.Signaling;
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
        Furioos
    }

    internal static class RenderStreamingSettings
    {
        public const int DefaultStreamWidth = 1280;
        public const int DefaultStreamHeight = 720;

        private static SignalingType s_signalingType = SignalingType.WebSocket;
        private static string s_signalingAddress = "localhost";
        private static float s_signalingInterval = 5;
        private static bool s_signalingSecured = false;
        private static Vector2Int s_streamSize = new Vector2Int(DefaultStreamWidth, DefaultStreamHeight);
        private static int s_selectVideoCodecIndex = -1;

        // todo(kazuki): This is be only a temporary measure.
        private static int s_selectSenderVideoCodecIndex = -1;

        public static SignalingType SignalingType
        {
            get { return s_signalingType; }
            set { s_signalingType = value; }
        }

        public static string SignalingAddress
        {
            get { return s_signalingAddress; }
            set { s_signalingAddress = value; }
        }

        public static bool SignalingSecured
        {
            get { return s_signalingSecured; }
            set { s_signalingSecured = value; }
        }

        public static float SignalingInterval
        {
            get { return s_signalingInterval; }
            set { s_signalingInterval = value; }
        }

        public static ISignaling Signaling
        {
            get
            {
                switch (s_signalingType)
                {
                    case SignalingType.Furioos:
                    {
                        var schema = s_signalingSecured ? "https" : "http";
                        return new FurioosSignaling(
                            $"{schema}://{s_signalingAddress}", s_signalingInterval, SynchronizationContext.Current);
                    }
                    case SignalingType.WebSocket:
                    {
                        var schema = s_signalingSecured ? "wss" : "ws";
                        return new WebSocketSignaling(
                            $"{schema}://{s_signalingAddress}", s_signalingInterval, SynchronizationContext.Current);
                    }
                    case SignalingType.Http:
                    {
                        var schema = s_signalingSecured ? "https" : "http";
                        return new HttpSignaling(
                            $"{schema}://{s_signalingAddress}", s_signalingInterval, SynchronizationContext.Current);
                    }
                }

                throw new InvalidOperationException();
            }
        }

        public static Vector2Int StreamSize
        {
            get { return s_streamSize; }
            set { s_streamSize = value; }
        }

        public static int SelectVideoCodecIndex
        {
            get { return s_selectVideoCodecIndex; }
            set { s_selectVideoCodecIndex = value; }
        }

        public static int SelectSenderVideoCodecIndex
        {
            get { return s_selectSenderVideoCodecIndex; }
            set { s_selectSenderVideoCodecIndex = value; }
        }
    }

    internal class SceneSelectUI : MonoBehaviour
    {
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

        [SerializeField] private List<Vector2Int> streamSizeList = new List<Vector2Int>
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

        void Start()
        {
            dropdownSignalingType.value = (int)RenderStreamingSettings.SignalingType;
            inputFieldSignalingAddress.text = RenderStreamingSettings.SignalingAddress;
            toggleSignalingSecured.isOn = RenderStreamingSettings.SignalingSecured;
            inputFieldSignalingInterval.text =
                RenderStreamingSettings.SignalingInterval.ToString(CultureInfo.InvariantCulture);

            dropdownSignalingType.onValueChanged.AddListener(OnChangeSignalingType);
            inputFieldSignalingAddress.onValueChanged.AddListener(OnChangeSignalingAddress);
            toggleSignalingSecured.onValueChanged.AddListener(OnChangeSignalingSecured);
            inputFieldSignalingInterval.onValueChanged.AddListener(OnChangeSignalingInterval);

            var optionList = streamSizeList.Select(size => new Dropdown.OptionData($" {size.x} x {size.y} ")).ToList();
            optionList.Add(new Dropdown.OptionData(" Custom "));
            streamSizeSelector.options = optionList;

            var existInList = streamSizeList.Contains(RenderStreamingSettings.StreamSize);
            if (existInList)
            {
                streamSizeSelector.value = streamSizeList.IndexOf(RenderStreamingSettings.StreamSize);
            }
            else
            {
                streamSizeSelector.value = optionList.Count - 1;
                textureWidthInput.text = RenderStreamingSettings.StreamSize.x.ToString();
                textureHeightInput.text = RenderStreamingSettings.StreamSize.y.ToString();
                textureWidthInput.interactable = true;
                textureHeightInput.interactable = true;
            }

            streamSizeSelector.onValueChanged.AddListener(OnChangeStreamSizeSelect);
            textureWidthInput.onValueChanged.AddListener(OnChangeTextureWidthInput);
            textureHeightInput.onValueChanged.AddListener(OnChangeTextureHeightInput);

            var videoCodecList = AvailableCodecsUtils.GetAvailableVideoCodecsName()
                .Select(pair => new Dropdown.OptionData(pair.Value)).ToList();
            receiverVideoCodecSelector.options.AddRange(videoCodecList);

            // todo(kazuki): This is be only a temporary measure.
            // AvailableCodecsUtils.GetAvailableVideoCodecsName returns list for receiver, not sender.
            senderVideoCodecSelector.options.AddRange(videoCodecList);

            if (RenderStreamingSettings.SelectVideoCodecIndex >= 0 &&
                RenderStreamingSettings.SelectVideoCodecIndex < videoCodecList.Count)
            {
                receiverVideoCodecSelector.value = RenderStreamingSettings.SelectVideoCodecIndex + 1;
            }
            if (RenderStreamingSettings.SelectSenderVideoCodecIndex >= 0 &&
                RenderStreamingSettings.SelectSenderVideoCodecIndex < videoCodecList.Count)
            {
                senderVideoCodecSelector.value = RenderStreamingSettings.SelectSenderVideoCodecIndex + 1;
            }

            receiverVideoCodecSelector.onValueChanged.AddListener(OnChangeVideoCodecSelect);
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
            RenderStreamingSettings.SignalingType =
                (SignalingType)Enum.GetValues(typeof(SignalingType)).GetValue(value);
        }

        private void OnChangeSignalingAddress(string value)
        {
            RenderStreamingSettings.SignalingAddress = value;
        }

        private void OnChangeSignalingSecured(bool value)
        {
            RenderStreamingSettings.SignalingSecured = value;
        }

        private void OnChangeSignalingInterval(string value)
        {
            if (float.TryParse(value, out float _value))
            {
                RenderStreamingSettings.SignalingInterval = _value;
            }
            else
            {
                RenderStreamingSettings.SignalingInterval = 5;
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

            RenderStreamingSettings.StreamSize = streamSizeList[index];
        }

        private void OnChangeTextureWidthInput(string input)
        {
            var height = RenderStreamingSettings.StreamSize.y;

            if (string.IsNullOrEmpty(input))
            {
                RenderStreamingSettings.StreamSize = new Vector2Int(RenderStreamingSettings.DefaultStreamWidth, height);
                return;
            }

            if (int.TryParse(input, out var width))
            {
                RenderStreamingSettings.StreamSize = new Vector2Int(width, height);
            }
        }

        private void OnChangeTextureHeightInput(string input)
        {
            var width = RenderStreamingSettings.StreamSize.x;

            if (string.IsNullOrEmpty(input))
            {
                RenderStreamingSettings.StreamSize = new Vector2Int(width, RenderStreamingSettings.DefaultStreamHeight);
                return;
            }

            if (int.TryParse(input, out var height))
            {
                RenderStreamingSettings.StreamSize = new Vector2Int(width, height);
            }
        }

        private void OnChangeVideoCodecSelect(int index)
        {
            RenderStreamingSettings.SelectVideoCodecIndex = index - 1;
        }

        private void OnChangeSenderVideoCodecSelect(int index)
        {
            RenderStreamingSettings.SelectSenderVideoCodecIndex = index - 1;
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
