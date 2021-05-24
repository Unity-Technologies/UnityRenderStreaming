using System;
using System.Collections;
using System.Threading;
using Unity.RenderStreaming.Samples;
using Unity.RenderStreaming.Signaling;
using UnityEngine;
using UnityEngine.SceneManagement;
using UnityEngine.UI;
using Unity.WebRTC;
using Gyroscope = UnityEngine.InputSystem.Gyroscope;

#if URS_USE_AR_FOUNDATION
using System;
using UnityEngine.XR.ARFoundation;
#endif

namespace Unity.RenderStreaming.Samples
{
    internal enum SignalingType
    {
        Furioos,
        WebSocket,
        Http
    }

    internal static class RenderStreamingSettings
    {
        private static bool s_enableHWCodec = false;
        private static SignalingType s_signalingType = SignalingType.WebSocket;
        private static string s_signalingAddress = "localhost";
        private static int s_signalingInterval = 5;
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

        [SerializeField] private Button buttonBidirectional;
        [SerializeField] private Button buttonBroadcast;
        [SerializeField] private Button buttonGyro;
        [SerializeField] private Button buttonRenderPipeline;
        [SerializeField] private Button buttonReceiver;
        [SerializeField] private Button buttonWebBrowserInput;
        [SerializeField] private Button buttonAR;

        void Start()
        {
            toggleEnableHWCodec.isOn = RenderStreamingSettings.EnableHWCodec;
            toggleEnableHWCodec.onValueChanged.AddListener(OnChangeHWCodec);
            dropdownSignalingType.onValueChanged.AddListener(OnChangeSignalingType);
            inputFieldSignalingAddress.onValueChanged.AddListener(OnChangeSignalingAddress);
            toggleSignalingSecured.onValueChanged.AddListener(OnChangeSignalingSecured);

            buttonBidirectional.onClick.AddListener(OnPressedBidirectional);
            buttonBroadcast.onClick.AddListener(OnPressedBroadcast);
            buttonGyro.onClick.AddListener(OnPressedGyro);
            buttonRenderPipeline.onClick.AddListener(OnPressedRenderPipeline);
            buttonReceiver.onClick.AddListener(OnPressedReceiver);
            buttonWebBrowserInput.onClick.AddListener(OnPressedWebBrowserInput);
            buttonAR.onClick.AddListener(OnPressedAR);


            // Gyro input is not supported on this device.
            if (Gyroscope.current == null)
            {
                buttonGyro.interactable = false;
            }

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
            string type = dropdownSignalingType.itemText.text;
            if (Enum.TryParse(type, out SignalingType result))
            {
                RenderStreamingSettings.SignalingType = result;
            }
        }

        private void OnChangeSignalingAddress(string value)
        {
            RenderStreamingSettings.SignalingAddress = value;
        }

        private void OnChangeSignalingSecured(bool value)
        {
            RenderStreamingSettings.SignalingSecured = value;
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
            SceneManager.LoadScene("MultiplePeerConnections", LoadSceneMode.Single);
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
    }
}
