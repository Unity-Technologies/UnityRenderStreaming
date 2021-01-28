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
#pragma warning restore 0649

        void Awake()
        {
            sendOfferButton.onClick.AddListener(SendOffer);
            connectionIdInput.onValueChanged.AddListener(input => receiveVideoViewer.ChangeConnectionId(input));
            receiveVideoViewer.OnUpdateReceiveTexture += texture => remoteVideoImage.texture = texture;
        }

        private void SendOffer()
        {
            receiveVideoViewer.enabled = true;
        }
    }
}
