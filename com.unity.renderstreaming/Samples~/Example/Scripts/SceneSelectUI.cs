using System;
using System.Collections;
using System.Globalization;
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
        private static bool s_enableHWCodec = false;
        private static SignalingType s_signalingType = SignalingType.WebSocket;
        private static string s_signalingAddress = "localhost";
        private static float s_signalingInterval = 5;
        private static bool s_signalingSecured = false;

        public static bool EnableHWCodec
        {
            get { return s_enableHWCodec; }
            set { s_enableHWCodec = value; }
        }

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
    }

    internal class SceneSelectUI : MonoBehaviour
    {
        [SerializeField] private Toggle toggleEnableHWCodec;
        [SerializeField] private Dropdown dropdownSignalingType;
        [SerializeField] private InputField inputFieldSignalingAddress;
        [SerializeField] private Toggle toggleSignalingSecured;
        [SerializeField] private InputField inputFieldSignalingInterval;

        [SerializeField] private Button buttonBidirectional;
        [SerializeField] private Button buttonBroadcast;
        [SerializeField] private Button buttonGyro;
        [SerializeField] private Button buttonRenderPipeline;
        [SerializeField] private Button buttonReceiver;
        [SerializeField] private Button buttonWebBrowserInput;
        [SerializeField] private Button buttonAR;
        [SerializeField] private Button buttonMultiplay;

        void Start()
        {
            toggleEnableHWCodec.isOn = RenderStreamingSettings.EnableHWCodec;
            dropdownSignalingType.value = (int)RenderStreamingSettings.SignalingType;
            inputFieldSignalingAddress.text = RenderStreamingSettings.SignalingAddress;
            toggleSignalingSecured.isOn = RenderStreamingSettings.SignalingSecured;
            inputFieldSignalingInterval.text = RenderStreamingSettings.SignalingInterval.ToString(CultureInfo.InvariantCulture);

            toggleEnableHWCodec.onValueChanged.AddListener(OnChangeHWCodec);
            dropdownSignalingType.onValueChanged.AddListener(OnChangeSignalingType);
            inputFieldSignalingAddress.onValueChanged.AddListener(OnChangeSignalingAddress);
            toggleSignalingSecured.onValueChanged.AddListener(OnChangeSignalingSecured);
            inputFieldSignalingInterval.onValueChanged.AddListener(OnChangeSignalingInterval);

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

        private void OnChangeHWCodec(bool enable)
        {
            RenderStreamingSettings.EnableHWCodec = enable;
        }

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
#if UNITY_2020_1_OR_NEWER
            SceneManager.LoadScene("ARFoundation4.0", LoadSceneMode.Single);
#else
            SceneManager.LoadScene("ARFoundation2.1", LoadSceneMode.Single);
#endif
        }

        private void OnPressedMultiplay()
        {
            SceneManager.LoadScene("Multiplay", LoadSceneMode.Single);
        }
    }
}
