using System.Linq;
using UnityEngine;
using UnityEngine.UI;

namespace Unity.RenderStreaming.Samples
{
    class BidirectionalSample : MonoBehaviour
    {
#pragma warning disable 0649
        [SerializeField] private RenderStreaming renderStreaming;
        [SerializeField] private Dropdown webcamSelectDropdown;
        [SerializeField] private Dropdown microphoneSelectDropdown;
        [SerializeField] private Button startButton;
        [SerializeField] private Button setUpButton;
        [SerializeField] private Button hangUpButton;
        [SerializeField] private InputField connectionIdInput;
        [SerializeField] private RawImage localVideoImage;
        [SerializeField] private RawImage remoteVideoImage;
        [SerializeField] private AudioSource receiveAudioSource;
        [SerializeField] private WebCamStreamSender webCamStreamer;
        [SerializeField] private VideoStreamReceiver receiveVideoViewer;
        [SerializeField] private MicrophoneStreamSender microphoneStreamer;
        [SerializeField] private AudioStreamReceiver receiveAudioViewer;
        [SerializeField] private SingleConnection singleConnection;
#pragma warning restore 0649

        private string connectionId;

        void Awake()
        {
            startButton.interactable = true;
            webcamSelectDropdown.interactable = true;
            setUpButton.interactable = false;
            hangUpButton.interactable = false;
            connectionIdInput.interactable = true;
            startButton.onClick.AddListener(() =>
            {
                webCamStreamer.enabled = true;
                startButton.interactable = false;
                webcamSelectDropdown.interactable = false;
                microphoneStreamer.enabled = true;
                microphoneSelectDropdown.interactable = false;
                setUpButton.interactable = true;
            });
            setUpButton.onClick.AddListener(SetUp);
            hangUpButton.onClick.AddListener(HangUp);
            connectionIdInput.onValueChanged.AddListener(input => connectionId = input);
            connectionIdInput.text = $"{Random.Range(0, 99999):D5}";
            webcamSelectDropdown.onValueChanged.AddListener(index => webCamStreamer.SetDeviceIndex(index));
            webcamSelectDropdown.options =
                webCamStreamer.WebCamNameList.Select(x => new Dropdown.OptionData(x)).ToList();
            webCamStreamer.OnStartedStream += id => receiveVideoViewer.enabled = true;
            webCamStreamer.OnUpdateWebCamTexture += texture => localVideoImage.texture = texture;
            receiveVideoViewer.OnUpdateReceiveTexture += texture => remoteVideoImage.texture = texture;
            microphoneSelectDropdown.onValueChanged.AddListener(index => microphoneStreamer.SetDeviceIndex(index));
            microphoneSelectDropdown.options =
                microphoneStreamer.MicrophoneNameList.Select(x => new Dropdown.OptionData(x)).ToList();
            microphoneStreamer.OnStartedStream += id => receiveAudioViewer.enabled = true;
            receiveAudioViewer.SetSource(receiveAudioSource);
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

            remoteVideoImage.texture = null;
            setUpButton.interactable = true;
            hangUpButton.interactable = false;
            connectionIdInput.interactable = true;
            connectionIdInput.text = $"{Random.Range(0, 99999):D5}";
        }
    }
}
