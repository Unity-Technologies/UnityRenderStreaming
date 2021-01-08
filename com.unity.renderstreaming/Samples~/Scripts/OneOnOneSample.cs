using UnityEngine;
using UnityEngine.UI;

namespace Unity.RenderStreaming
{
    public class OneOnOneSample : MonoBehaviour
    {
#pragma warning disable 0649
        [SerializeField] private Button setUpButton;
        [SerializeField] private Button hangUpButton;
        [SerializeField] private InputField connectionIdInput;
        [SerializeField] private RawImage localVideoImage;
        [SerializeField] private RawImage remoteVideoImage;
        [SerializeField] private VideoStreamBase videoStream;
        [SerializeField] private ReceiveVideoViewer receiveVideoViewer;
#pragma warning restore 0649

        void Awake()
        {
            setUpButton.interactable = true;
            hangUpButton.interactable = false;
            connectionIdInput.interactable = true;
            setUpButton.onClick.AddListener(SetUp);
            hangUpButton.onClick.AddListener(HangUp);
            connectionIdInput.onValueChanged.AddListener(input => receiveVideoViewer.ChangeConnectionId(input));
            connectionIdInput.text = $"{Random.Range(0, 99999):D5}";
            videoStream.OnEnableComplete += () => {
                receiveVideoViewer.enabled = true;
                localVideoImage.texture = videoStream.SendTexture;
            };
            receiveVideoViewer.OnUpdateReceiveTexture += texture => remoteVideoImage.texture = texture;
        }

        private void SetUp()
        {
            setUpButton.interactable = false;
            hangUpButton.interactable = true;
            connectionIdInput.interactable = false;
            videoStream.enabled = true;
        }

        private void HangUp()
        {
            videoStream.enabled = false;
            receiveVideoViewer.enabled = false;
            localVideoImage.texture = null;
            remoteVideoImage.texture = null;
            setUpButton.interactable = true;
            hangUpButton.interactable = false;
            connectionIdInput.interactable = true;
            connectionIdInput.text = $"{Random.Range(0, 99999):D5}";
        }
    }
}
