using UnityEngine;
using UnityEngine.UI;

namespace Unity.RenderStreaming
{
    public class OneOnOneSample : MonoBehaviour
    {
#pragma warning disable 0649
        [SerializeField] private Button setUpButton;
        [SerializeField] private Button hangUpButton;
        [SerializeField] private WebCamStreamer webCamStreamer;
        [SerializeField] private ReceiveVideoViewer receiveVideoViewer;
#pragma warning restore 0649

        void Awake()
        {
            setUpButton.onClick.AddListener(SetUp);
            hangUpButton.onClick.AddListener(HangUp);
        }

        void OnDestroy()
        {
            HangUp();
        }


        private void SetUp()
        {
            RenderStreaming.Instance.enabled = true;
            webCamStreamer.enabled = true;
            receiveVideoViewer.enabled = true;
        }

        private void HangUp()
        {
            webCamStreamer.enabled = false;
            receiveVideoViewer.enabled = false;
            RenderStreaming.Instance.enabled = false;
        }
    }
}
