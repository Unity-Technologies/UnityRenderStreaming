using System;
using UnityEngine;
using UnityEngine.UI;

namespace Unity.RenderStreaming.Samples
{
    static class InputSenderExtension
    {
        public static (Rect, Vector2Int) GetRegionAndSize(this RawImage image)
        {
            // correct pointer position
            Vector3[] corners = new Vector3[4];
            image.rectTransform.GetWorldCorners(corners);
            Camera camera = image.canvas.worldCamera;
            var corner0 = RectTransformUtility.WorldToScreenPoint(camera, corners[0]);
            var corner2 = RectTransformUtility.WorldToScreenPoint(camera, corners[2]);
            var region = new Rect(
                corner0.x,
                corner0.y,
                corner2.x - corner0.x,
                corner2.y - corner0.y
                );

            var size = new Vector2Int(image.texture.width, image.texture.height);
            return (region, size);
        }
    }

    class ReceiverSample : MonoBehaviour
    {
#pragma warning disable 0649
        [SerializeField] private SignalingManager renderStreaming;
        [SerializeField] private Button startButton;
        [SerializeField] private Button stopButton;
        [SerializeField] private InputField connectionIdInput;
        [SerializeField] private RawImage remoteVideoImage;
        [SerializeField] private AudioSource remoteAudioSource;
        [SerializeField] private VideoStreamReceiver receiveVideoViewer;
        [SerializeField] private AudioStreamReceiver receiveAudioViewer;
        [SerializeField] private SingleConnection connection;
        [SerializeField] private Text resolution;
#pragma warning restore 0649

        private string connectionId;
        private InputSender inputSender;
        private RenderStreamingSettings settings;
        private Vector2 lastSize;

        void Awake()
        {
            startButton.onClick.AddListener(OnStart);
            stopButton.onClick.AddListener(OnStop);
            if(connectionIdInput != null)
                connectionIdInput.onValueChanged.AddListener(input => connectionId = input);

            receiveVideoViewer.OnUpdateReceiveTexture += OnUpdateReceiveTexture;
            receiveAudioViewer.OnUpdateReceiveAudioSource += source =>
            {
                source.loop = true;
                source.Play();
            };

            inputSender = GetComponent<InputSender>();
            inputSender.OnStartedChannel += OnStartedChannel;

            settings = SampleManager.Instance.Settings;
        }

        void Start()
        {
            if (renderStreaming.runOnAwake)
                return;

            if (settings != null)
                renderStreaming.useDefaultSettings = settings.UseDefaultSettings;
            if (settings?.SignalingSettings != null)
                renderStreaming.SetSignalingSettings(settings.SignalingSettings);
            renderStreaming.Run();
        }

        private void Update()
        {
            // Call SetInputChange if window size is changed.
            var size = remoteVideoImage.rectTransform.sizeDelta;
            if (lastSize == size)
                return;
            lastSize = size;
            CalculateInputRegion();
        }

        void OnUpdateReceiveTexture(Texture texture)
        {
            remoteVideoImage.texture = texture;
            CalculateInputRegion();
        }

        void OnStartedChannel(string connectionId)
        {
            CalculateInputRegion();
        }

        private void OnRectTransformDimensionsChange()
        {
            CalculateInputRegion();
        }

        void CalculateInputRegion()
        {
            if (inputSender == null || !inputSender.IsConnected || remoteVideoImage.texture == null)
                return;
            var (region, size) = remoteVideoImage.GetRegionAndSize();
            resolution.text = $"{(int)region.width} x {(int)region.height}";
            inputSender.CalculateInputResion(region, size);
            inputSender.EnableInputPositionCorrection(true);
        }

        private void OnStart()
        {
            if (string.IsNullOrEmpty(connectionId))
            {
                connectionId = System.Guid.NewGuid().ToString("N");
                connectionIdInput.text = connectionId;
            }
            connectionIdInput.interactable = false;
            if(settings != null)
                receiveVideoViewer.SetCodec(settings.ReceiverVideoCodec);
            receiveAudioViewer.targetAudioSource = remoteAudioSource;

            connection.CreateConnection(connectionId);
            startButton.gameObject.SetActive(false);
            stopButton.gameObject.SetActive(true);
        }

        private void OnStop()
        {
            connection.DeleteConnection(connectionId);
            connectionId = String.Empty;
            connectionIdInput.text = String.Empty;
            connectionIdInput.interactable = true;
            startButton.gameObject.SetActive(true);
            stopButton.gameObject.SetActive(false);
        }
    }
}
