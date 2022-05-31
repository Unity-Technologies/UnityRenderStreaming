using System;
using UnityEngine;
using UnityEngine.UI;

namespace Unity.RenderStreaming.Samples
{
    static class InputSenderExtension
    {
        public static void SetInputRange(this InputSender sender, RawImage image)
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
            sender.SetInputRange(region, size);
        }
    }

    class ReceiverSample : MonoBehaviour
    {
#pragma warning disable 0649
        [SerializeField] private RenderStreaming renderStreaming;
        [SerializeField] private Button startButton;
        [SerializeField] private Button stopButton;
        [SerializeField] private InputField connectionIdInput;
        [SerializeField] private RawImage remoteVideoImage;
        [SerializeField] private AudioSource remoteAudioSource;
        [SerializeField] private VideoStreamReceiver receiveVideoViewer;
        [SerializeField] private AudioStreamReceiver receiveAudioViewer;
        [SerializeField] private SingleConnection connection;
        [SerializeField] private VideoCodecSelect videoCodecSelect;
#pragma warning restore 0649

        private string connectionId;
        private InputSender inputSender;

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
        }

        void Start()
        {
            if (renderStreaming.runOnAwake)
                return;
            renderStreaming.Run(signaling: RenderStreamingSettings.Signaling);
            inputSender = GetComponent<InputSender>();
            inputSender.OnStartedChannel += OnStartedChannel;
            videoCodecSelect.enabled = true;
            videoCodecSelect.ChangeInteractable(true);
        }

        void OnUpdateReceiveTexture(Texture texture)
        {
            remoteVideoImage.texture = texture;
            SetInputChange();
        }

        void OnStartedChannel(string connectionId)
        {
            SetInputChange();
        }

        void SetInputChange()
        {
            if (!inputSender.IsConnected || remoteVideoImage.texture == null)
                return;
            inputSender.SetInputRange(remoteVideoImage);
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
            videoCodecSelect.ChangeInteractable(false);
            receiveVideoViewer.FilterVideoCodecs(videoCodecSelect.SelectIndex);
            receiveAudioViewer.SetSource(remoteAudioSource);

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
            videoCodecSelect.ChangeInteractable(true);
            startButton.gameObject.SetActive(true);
            stopButton.gameObject.SetActive(false);
        }
    }
}
