using UnityEngine;
using UnityEngine.UI;

namespace Unity.RenderStreaming.Samples
{
    class BidirectionalSample : MonoBehaviour
    {
#pragma warning disable 0649
        [SerializeField] private RenderStreaming renderStreaming;
        [SerializeField] private Button setUpButton;
        [SerializeField] private Button hangUpButton;
        [SerializeField] private InputField connectionIdInput;
        [SerializeField] private RawImage localVideoImage;
        [SerializeField] private RawImage remoteVideoImage;
        [SerializeField] private VideoStreamBase videoStream;
        [SerializeField] private ReceiveVideoViewer receiveVideoViewer;
        [SerializeField] private SingleConnection singleConnection;
#pragma warning restore 0649

        private string connectionId;

        void Awake()
        {
            setUpButton.interactable = true;
            hangUpButton.interactable = true;
            connectionIdInput.interactable = true;
            setUpButton.onClick.AddListener(SetUp);
            hangUpButton.onClick.AddListener(HangUp);
            connectionIdInput.onValueChanged.AddListener(input => connectionId = input);
            connectionIdInput.text = $"{Random.Range(0, 99999):D5}";
            videoStream.OnStartedStream += connectionId =>
            {
                receiveVideoViewer.enabled = true;
                localVideoImage.texture = videoStream.SendTexture;
            };
            receiveVideoViewer.OnUpdateReceiveTexture += texture => remoteVideoImage.texture = texture;
        }

        void Start()
        {
            if (renderStreaming.runOnAwake)
                return;
            renderStreaming.Run(
                hardwareEncoder: RenderStreamingSettings.EnableHWCodec,
                signaling: RenderStreamingSettings.Signaling);
        }

        private void SetUp()
        {
            setUpButton.interactable = false;
            hangUpButton.interactable = true;
            connectionIdInput.interactable = false;

            singleConnection.CreateConnection(connectionId);
        }

        private void HangUp()
        {
            singleConnection.DeleteConnection(connectionId);

            localVideoImage.texture = null;
            remoteVideoImage.texture = null;
            setUpButton.interactable = true;
            hangUpButton.interactable = false;
            connectionIdInput.interactable = true;
            connectionIdInput.text = $"{Random.Range(0, 99999):D5}";
        }
    }
}
