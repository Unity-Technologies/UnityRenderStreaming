using System.Collections;
using UnityEngine;
using UnityEngine.UI;

namespace Unity.RenderStreaming
{
    public class ReceiveVideoSample : MonoBehaviour
    {
#pragma warning disable 0649
        [SerializeField] private Button sendOfferButton;
        [SerializeField] private InputField connectionIdInput;
        [SerializeField] private RawImage remoteVideoImage;
        [SerializeField] private ReceiveVideoViewer receiveVideoViewer;
        [SerializeField] private SingleConnection connection;
#pragma warning restore 0649

        private string connectionId;

        void Awake()
        {
            sendOfferButton.onClick.AddListener(SendOffer);
            connectionIdInput.onValueChanged.AddListener(input => connectionId = input);
            receiveVideoViewer.OnUpdateReceiveTexture += texture => remoteVideoImage.texture = texture;
        }

        private void SendOffer()
        {
            connection.CreateConnection(connectionId, true);
        }
    }
}
