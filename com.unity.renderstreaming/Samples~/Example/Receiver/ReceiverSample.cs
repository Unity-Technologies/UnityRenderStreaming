using System;
using Unity.RenderStreaming.Samples;
using UnityEngine;
using UnityEngine.UI;

namespace Unity.RenderStreaming.Samples
{
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
#pragma warning restore 0649

        private string connectionId;

        void Awake()
        {
            startButton.onClick.AddListener(OnStart);
            stopButton.onClick.AddListener(OnStop);
            if(connectionIdInput != null)
                connectionIdInput.onValueChanged.AddListener(input => connectionId = input);

            receiveVideoViewer.OnUpdateReceiveTexture += OnUpdateReceiveTexture;
            receiveAudioViewer.SetSource(remoteAudioSource);
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
            renderStreaming.Run(
                hardwareEncoder: RenderStreamingSettings.EnableHWCodec,
                signaling: RenderStreamingSettings.Signaling);
        }

        void OnUpdateReceiveTexture(Texture texture)
        {
            remoteVideoImage.texture = texture;

            // correct pointer position
            InputSender inputSender = GetComponent<InputSender>();
            Vector3[] corners = new Vector3[4];
            remoteVideoImage.rectTransform.GetWorldCorners(corners);
            Camera camera = remoteVideoImage.canvas.worldCamera;
            var corner0 = RectTransformUtility.WorldToScreenPoint(camera, corners[0]);
            var corner2 = RectTransformUtility.WorldToScreenPoint(camera, corners[2]);

            var rect = new Rect(
                corner0.x,
                corner0.y,
                corner2.x - corner0.x,
                corner2.y - corner0.y
                );

            // todo(kazuki)::
            // This texture size is determined receiver side, so the
            // correction process is not work correctly.
            // We should fix that the sender size determine the size.
            var size = new Vector2Int(texture.width, texture.height);
            inputSender.SetCorrectPointerPositionInfo(size, rect);
            inputSender.EnableCorrectPointerPosition(true);
        }

        private void OnStart()
        {
            if (string.IsNullOrEmpty(connectionId))
            {
                connectionId = System.Guid.NewGuid().ToString("N");
                connectionIdInput.text = connectionId;
            }
            connectionIdInput.interactable = false;

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
