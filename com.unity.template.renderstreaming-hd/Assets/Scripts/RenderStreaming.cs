using System;
using System.Collections.Generic;
using System.Linq;
using UnityEngine;
using Unity.WebRTC;
using System.Text.RegularExpressions;
using Unity.RenderStreaming.Signaling;
using UnityEngine.InputSystem.EnhancedTouch;

namespace Unity.RenderStreaming
{
    [Serializable]
    public class ButtonClickEvent : UnityEngine.Events.UnityEvent<int> { }

    [Serializable]
    public class ButtonClickElement
    {
        [Tooltip("Specifies the ID on the HTML")]
        public int elementId;
        public ButtonClickEvent click;
    }

    public class RenderStreaming : MonoBehaviour
    {
#pragma warning disable 0649
        [SerializeField, Tooltip("Address for signaling server")]
        private string urlSignaling = "http://localhost";

        [SerializeField, Tooltip("Array to set your own STUN/TURN servers")]
        private RTCIceServer[] iceServers = new RTCIceServer[]
        {
            new RTCIceServer()
            {
                urls = new string[] { "stun:stun.l.google.com:19302" }
            }
        };

        [SerializeField, Tooltip("Streaming size should match display aspect ratio")]
        private Vector2Int streamingSize = new Vector2Int(1280, 720);

        [SerializeField, Tooltip("Streaming bit rate")]
        private int bitRate = 1000000;

        [SerializeField, Tooltip("Time interval for polling from signaling server")]
        private float interval = 5.0f;

        [SerializeField, Tooltip("Camera to capture video stream")]
        private Camera captureCamera;

        [SerializeField, Tooltip("Enable or disable hardware encoder")]
        private bool hardwareEncoderSupport = true;

        [SerializeField, Tooltip("Array to set your own click event")]
        private ButtonClickElement[] arrayButtonClickEvent;
#pragma warning restore 0649

        private ISignaling signaling;
        private readonly Dictionary<string, RTCPeerConnection> pcs = new Dictionary<string, RTCPeerConnection>();
        private readonly Dictionary<RTCPeerConnection, Dictionary<int, RTCDataChannel>> mapChannels = new Dictionary<RTCPeerConnection, Dictionary<int, RTCDataChannel>>();
        private readonly Dictionary<RemoteInput, SimpleCameraController> m_remoteInputAndCameraController = new Dictionary<RemoteInput, SimpleCameraController>();
        private readonly Dictionary<RTCDataChannel, RemoteInput> m_channelIdAndRemoteInput = new Dictionary<RTCDataChannel, RemoteInput>();
        private readonly List<SimpleCameraController> m_listController = new List<SimpleCameraController>();
        private RTCConfiguration conf;
        private MediaStream videoStream;
        private MediaStream audioStream;
        private DefaultInput m_defaultInput;

        public static RenderStreaming Instance { get; private set; }

        public void Awake()
        {
            Instance = this;
            var encoderType = hardwareEncoderSupport ? EncoderType.Hardware : EncoderType.Software;
            WebRTC.WebRTC.Initialize(encoderType);
            m_defaultInput = new DefaultInput();
            EnhancedTouchSupport.Enable();
        }

        public void OnDestroy()
        {
            Instance = null;
            EnhancedTouchSupport.Disable();
            WebRTC.WebRTC.Dispose();
            RemoteInputReceiver.Dispose();
            Unity.WebRTC.Audio.Stop();
        }
        public void Start()
        {
            videoStream = captureCamera.CaptureStream(streamingSize.x, streamingSize.y, bitRate);
            audioStream = Unity.WebRTC.Audio.CaptureStream();
            conf = default;
            conf.iceServers = iceServers;
            StartCoroutine(WebRTC.WebRTC.Update());
        }

        void OnEnable()
        {
            if (this.signaling == null)
            {
                if (urlSignaling.StartsWith("ws"))
                {
                    this.signaling = new WebSocketSignaling(urlSignaling, interval);
                }
                else
                {
                    this.signaling = new HttpSignaling(urlSignaling, interval);
                }

                this.signaling.OnOffer += OnOffer;
                this.signaling.OnIceCandidate += OnIceCandidate;
            }
            this.signaling.Start();
        }

        public void AddController(SimpleCameraController controller)
        {
            m_listController.Add(controller);
            controller.SetInput(m_defaultInput);
        }

        public void RemoveController(SimpleCameraController controller)
        {
            m_listController.Remove(controller);
        }

        void OnDisable()
        {
            if (this.signaling != null)
            {
                this.signaling.Stop();
                this.signaling = null;
            }
        }

        public Vector2Int GetStreamingSize() { return streamingSize; }


        void OnOffer(ISignaling signaling, DescData e)
        {
            RTCSessionDescription _desc;
            _desc.type = RTCSdpType.Offer;
            _desc.sdp = e.sdp;
            var connectionId = e.connectionId;
            if (pcs.ContainsKey(connectionId))
            {
                return;
            }
            var pc = new RTCPeerConnection();
            pcs.Add(e.connectionId, pc);

            pc.OnDataChannel = new DelegateOnDataChannel(channel => { OnDataChannel(pc, channel); });
            pc.SetConfiguration(ref conf);
            pc.OnIceCandidate = new DelegateOnIceCandidate(candidate =>
            {
                signaling.SendCandidate(e.connectionId, candidate);
            });
            pc.OnIceConnectionChange = new DelegateOnIceConnectionChange(state =>
            {
                if(state == RTCIceConnectionState.Disconnected)
                {
                    pc.Close();
                    pcs.Remove(e.connectionId);
                }
            });
            //make video bit rate starts at 16000kbits, and 160000kbits at max.
            string pattern = @"(a=fmtp:\d+ .*level-asymmetry-allowed=.*)\r\n";
            _desc.sdp = Regex.Replace(_desc.sdp, pattern, "$1;x-google-start-bitrate=16000;x-google-max-bitrate=160000\r\n");
            pc.SetRemoteDescription(ref _desc);
            foreach (var track in videoStream.GetTracks())
            {
                pc.AddTrack(track);
            }
            foreach(var track in audioStream.GetTracks())
            {
                pc.AddTrack(track);
            }

            RTCAnswerOptions options = default;
            var op = pc.CreateAnswer(ref options);
            while (op.MoveNext())
            {
            }
            if (op.IsError)
            {
                Debug.LogError($"Network Error: {op.Error}");
                return;
            }

            var desc = op.Desc;
            var opLocalDesc = pc.SetLocalDescription(ref desc);
            while (opLocalDesc.MoveNext())
            {
            }
            if (opLocalDesc.IsError)
            {
                Debug.LogError($"Network Error: {opLocalDesc.Error}");
                return;
            }

            signaling.SendAnswer(connectionId, desc);
        }

        void OnIceCandidate(ISignaling signaling, CandidateData e)
        {
            if (!pcs.TryGetValue(e.connectionId, out var pc))
            {
                return;
            }

            RTCIceCandidate​ _candidate = default;
            _candidate.candidate = e.candidate;
            _candidate.sdpMLineIndex = e.sdpMLineIndex;
            _candidate.sdpMid = e.sdpMid;

            pc.AddIceCandidate(ref _candidate);
        }

        void OnDataChannel(RTCPeerConnection pc, RTCDataChannel channel)
        {
            if (!mapChannels.TryGetValue(pc, out var channels))
            {
                channels = new Dictionary<int, RTCDataChannel>();
                mapChannels.Add(pc, channels);
            }
            channels.Add(channel.Id, channel);

            if (channel.Label != "data")
            {
                return;
            }

            RemoteInput input = RemoteInputReceiver.Create();

            // device.current must be changed after creating devices
            m_defaultInput.MakeCurrent();

            m_channelIdAndRemoteInput.Add(channel, input);
            channel.OnMessage = bytes => m_channelIdAndRemoteInput[channel].ProcessInput(bytes);
            channel.OnClose = () => OnCloseChannel(channel);

            SimpleCameraController controller = m_listController
                .FirstOrDefault(_controller => !m_remoteInputAndCameraController.ContainsValue(_controller));

            if(controller != null)
            {
                controller.SetInput(input);
                m_remoteInputAndCameraController.Add(input, controller);
            }
        }

        void OnCloseChannel(RTCDataChannel channel)
        {
            RemoteInput input = m_channelIdAndRemoteInput[channel];
            RemoteInputReceiver.Delete(input);

            // device.current must be changed after removing devices
            m_defaultInput.MakeCurrent();

            if (m_remoteInputAndCameraController.TryGetValue(input, out var controller))
            {
                RemoteInput newInput = FindPrioritizedInput();
                if (newInput == null)
                {
                    controller.SetInput(m_defaultInput);
                }
                else
                {
                    controller.SetInput(newInput);
                    m_remoteInputAndCameraController.Add(newInput, controller);
                }
            }
            m_remoteInputAndCameraController.Remove(input);

            m_channelIdAndRemoteInput.Remove(channel);
        }

        RemoteInput FindPrioritizedInput()
        {
            var list = RemoteInputReceiver.All();

            // filter here
            // return null if not found the input
            return list.Except(m_remoteInputAndCameraController.Keys).FirstOrDefault();
        }

        void OnButtonClick(int elementId)
        {
            foreach (var element in arrayButtonClickEvent)
            {
                if (element.elementId == elementId)
                {
                    element.click.Invoke(elementId);
                }
            }
        }
    }
}
