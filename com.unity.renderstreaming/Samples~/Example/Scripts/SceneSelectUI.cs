using UnityEngine;
using UnityEngine.SceneManagement;
using UnityEngine.UI;
using Unity.WebRTC;

namespace Unity.RenderStreaming.Samples
{
    internal static class RenderStreamingSettings
    {
        private static bool s_enableHWCodec = false;

        public static bool EnableHWCodec
        {
            get { return s_enableHWCodec; }
            set { s_enableHWCodec = value; }
        }

        public static EncoderType EncoderType
        {
            get { return s_enableHWCodec ? EncoderType.Hardware : EncoderType.Software; }
        }
    }

    internal class SceneSelectUI : MonoBehaviour
    {
        [SerializeField] private Toggle toggleEnableHWCodec;
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

            buttonBidirectional.onClick.AddListener(OnPressedBidirectional);
            buttonBroadcast.onClick.AddListener(OnPressedBroadcast);
            buttonGyro.onClick.AddListener(OnPressedGyro);
            buttonRenderPipeline.onClick.AddListener(OnPressedRenderPipeline);
            buttonReceiver.onClick.AddListener(OnPressedReceiver);
            buttonWebBrowserInput.onClick.AddListener(OnPressedWebBrowserInput);
            buttonAR.onClick.AddListener(OnPressedAR);
        }

        private void OnChangeHWCodec(bool enable)
        {
            RenderStreamingSettings.EnableHWCodec = enable;
        }

        public void OnPressedBidirectional()
        {
            SceneManager.LoadScene("Bidirectional", LoadSceneMode.Single);
        }
        public void OnPressedBroadcast()
        {
            SceneManager.LoadScene("Broadcast", LoadSceneMode.Single);
        }
        public void OnPressedGyro()
        {
            SceneManager.LoadScene("Gyro", LoadSceneMode.Single);
        }
        public void OnPressedRenderPipeline()
        {
            SceneManager.LoadScene("MultiplePeerConnections", LoadSceneMode.Single);
        }
        public void OnPressedReceiver()
        {
            SceneManager.LoadScene("Receiver", LoadSceneMode.Single);
        }
        public void OnPressedWebBrowserInput()
        {
            SceneManager.LoadScene("WebBrowserInput", LoadSceneMode.Single);
        }
        public void OnPressedAR()
        {
#if UNITY_2020_1_OR_NEWER
            SceneManager.LoadScene("ARFoundation4.0", LoadSceneMode.Single);
#else
            SceneManager.LoadScene("ARFoundation2.1", LoadSceneMode.Single);
#endif
        }
    }
}
