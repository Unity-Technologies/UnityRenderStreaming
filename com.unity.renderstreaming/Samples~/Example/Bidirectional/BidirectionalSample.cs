using System.Linq;
using UnityEngine;
using UnityEngine.UI;

namespace Unity.RenderStreaming.Samples
{
    class BidirectionalSample : MonoBehaviour
    {
#pragma warning disable 0649
        [SerializeField] private SignalingManager renderStreaming;
        [SerializeField] private Dropdown webcamSelectDropdown;
        [SerializeField] private Dropdown microphoneSelectDropdown;
        [SerializeField] private Button startButton;
        [SerializeField] private Button setUpButton;
        [SerializeField] private Button hangUpButton;
        [SerializeField] private InputField connectionIdInput;
        [SerializeField] private RawImage localVideoImage;
        [SerializeField] private RawImage remoteVideoImage;
        [SerializeField] private AudioSource receiveAudioSource;
        [SerializeField] private VideoStreamSender webCamStreamer;
        [SerializeField] private VideoStreamReceiver receiveVideoViewer;
        [SerializeField] private AudioStreamSender microphoneStreamer;
        [SerializeField] private AudioStreamReceiver receiveAudioViewer;
        [SerializeField] private SingleConnection singleConnection;
#pragma warning restore 0649

        private string connectionId;
        private RenderStreamingSettings settings;

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
            webcamSelectDropdown.onValueChanged.AddListener(index => webCamStreamer.sourceDeviceIndex = index);
            webcamSelectDropdown.options = WebCamTexture.devices.Select(x => x.name).Select(x => new Dropdown.OptionData(x)).ToList();
            webCamStreamer.OnStartedStream += id => receiveVideoViewer.enabled = true;
            webCamStreamer.OnStartedStream += _ => localVideoImage.texture = webCamStreamer.sourceWebCamTexture;

            settings = SampleManager.Instance.Settings;
            if (settings != null)
            {
                webCamStreamer.width = (uint)settings.StreamSize.x;
                webCamStreamer.height = (uint)settings.StreamSize.y;
            }

            receiveVideoViewer.OnUpdateReceiveTexture += texture => remoteVideoImage.texture = texture;

            microphoneSelectDropdown.onValueChanged.AddListener(index => microphoneStreamer.sourceDeviceIndex = index);
            microphoneSelectDropdown.options =
                Microphone.devices.Select(x => new Dropdown.OptionData(x)).ToList();
            receiveAudioViewer.targetAudioSource = receiveAudioSource;
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
            if (settings != null)
                renderStreaming.useDefaultSettings = settings.UseDefaultSettings;
            if (settings?.SignalingSettings != null)
                renderStreaming.SetSignalingSettings(settings.SignalingSettings);
            renderStreaming.Run();
        }

        private void SetUp()
        {
            setUpButton.interactable = false;
            hangUpButton.interactable = true;
            connectionIdInput.interactable = false;
            if(settings != null)
            {
                receiveVideoViewer.SetCodec(settings.ReceiverVideoCodec);
                webCamStreamer.SetCodec(settings.SenderVideoCodec);
            }

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
            localVideoImage.texture = null;
        }
    }
}
