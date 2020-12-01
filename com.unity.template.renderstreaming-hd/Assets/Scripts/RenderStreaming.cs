using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Threading;
using UnityEngine;
using Unity.WebRTC;
using Unity.RenderStreaming.Signaling;
using UnityEngine.InputSystem.EnhancedTouch;

namespace Unity.RenderStreaming
{
    using DataChannelDictionary = Dictionary<int, RTCDataChannel>;

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
        [SerializeField, Tooltip("Signaling server url")]
        private string urlSignaling = "http://localhost";

        [SerializeField, Tooltip("Type of signaling server")]
        private string signalingType = typeof(HttpSignaling).FullName;

        [SerializeField, Tooltip("Array to set your own STUN/TURN servers")]
        private RTCIceServer[] iceServers = new RTCIceServer[]
        {
            new RTCIceServer()
            {
                urls = new string[] { "stun:stun.l.google.com:19302" }
            }
        };

        [SerializeField, Tooltip("Time interval for polling from signaling server")]
        private float interval = 5.0f;

        [SerializeField, Tooltip("Enable or disable hardware encoder")]
        private bool hardwareEncoderSupport = true;

        [SerializeField, Tooltip("Array to set your own click event")]
        private ButtonClickElement[] arrayButtonClickEvent;

#pragma warning restore 0649

        private SynchronizationContext m_mainThreadContext;
        private ISignaling m_signaling;
        private readonly Dictionary<string, RTCPeerConnection> m_mapConnectionIdAndPeer = new Dictionary<string, RTCPeerConnection>();
        private readonly Dictionary<RTCPeerConnection, DataChannelDictionary> m_mapPeerAndChannelDictionary = new Dictionary<RTCPeerConnection, DataChannelDictionary>();
        private readonly Dictionary<RemoteInput, SimpleCameraController> m_remoteInputAndCameraController = new Dictionary<RemoteInput, SimpleCameraController>();
        private readonly Dictionary<RTCDataChannel, RemoteInput> m_mapChannelAndRemoteInput = new Dictionary<RTCDataChannel, RemoteInput>();
        private readonly List<SimpleCameraController> m_listController = new List<SimpleCameraController>();
        private readonly List<VideoStreamTrack> m_listVideoStreamTrack = new List<VideoStreamTrack>();
        private readonly Dictionary<MediaStreamTrack, List<RTCRtpSender>> m_mapTrackAndSenderList = new Dictionary<MediaStreamTrack, List<RTCRtpSender>>();
        private readonly List<MediaStream> m_listVideoReceiveStream = new List<MediaStream>();
        private MediaStream m_audioStream;
        private DefaultInput m_defaultInput;
        private RTCConfiguration m_conf;
        private string m_connectionId;

        public static RenderStreaming Instance { get; private set; }

        enum UnityEventType
        {
            SwitchVideo = 0
        }


        public void Awake()
        {
            Instance = this;
            var encoderType = hardwareEncoderSupport ? EncoderType.Hardware : EncoderType.Software;
            WebRTC.WebRTC.Initialize(encoderType);
            m_defaultInput = new DefaultInput();
            EnhancedTouchSupport.Enable();
            m_mainThreadContext = SynchronizationContext.Current;
        }

        public void OnDestroy()
        {
            Instance = null;
            EnhancedTouchSupport.Disable();
            WebRTC.WebRTC.Dispose();
            RemoteInputReceiver.Dispose();
            Unity.WebRTC.Audio.Stop();
            m_mainThreadContext = null;
        }
        public void Start()
        {
            m_audioStream = Unity.WebRTC.Audio.CaptureStream();
            m_conf = default;
            m_conf.iceServers = iceServers;
            StartCoroutine(WebRTC.WebRTC.Update());
        }

        void OnEnable()
        {
            if (this.m_signaling == null)
            {
                Type t = Type.GetType(signalingType);
                object[] args = { urlSignaling, interval, m_mainThreadContext };
                this.m_signaling = (ISignaling)Activator.CreateInstance(t, args);
                this.m_signaling.OnStart += signaling => signaling.CreateConnection();
                this.m_signaling.OnCreateConnection += (signaling, id) =>
                {
                    m_connectionId = id;
                    CreatePeerConnection(signaling, m_connectionId, true);
                };
                this.m_signaling.OnOffer += (signaling, data) => StartCoroutine(OnOffer(signaling, data));
                this.m_signaling.OnAnswer += (signaling, data) => StartCoroutine(OnAnswer(signaling, data));
                this.m_signaling.OnIceCandidate += OnIceCandidate;
            }
            this.m_signaling.Start();
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

        public void AddVideoStreamTrack(VideoStreamTrack track)
        {
            m_listVideoStreamTrack.Add(track);
        }

        public void RemoveVideoStreamTrack(VideoStreamTrack track)
        {
            m_listVideoStreamTrack.Remove(track);
        }

        public void AddVideoReceiveStream(MediaStream stream)
        {
            m_listVideoReceiveStream.Add(stream);
        }

        public void RemoveVideoReceiveStream(MediaStream stream)
        {
            m_listVideoReceiveStream.Remove(stream);
        }

        public void AddTransceiver()
        {
            if (string.IsNullOrEmpty(m_connectionId) ||
                !m_mapConnectionIdAndPeer.TryGetValue(m_connectionId, out var pc))
            {
                return;
            }

            RTCRtpTransceiver transceiver = pc.AddTransceiver(TrackKind.Video);
            // ToDO: need webrtc package version 2.3
            // transceiver.Direction = RTCRtpTransceiverDirection.RecvOnly;
        }

        public void ChangeVideoParameters(VideoStreamTrack track, ulong? bitrate, uint? framerate)
        {
            foreach (var sender in m_mapTrackAndSenderList[track])
            {
                RTCRtpSendParameters parameters = sender.GetParameters();
                foreach (var encoding in parameters.Encodings)
                {
                    if(bitrate != null) encoding.maxBitrate = bitrate;
                    if (framerate != null) encoding.maxFramerate = framerate;
                }
                sender.SetParameters(parameters);
            }
        }

        void OnDisable()
        {
            if (this.m_signaling != null)
            {
                this.m_signaling.Stop();
                this.m_signaling = null;
            }
        }

        IEnumerator OnOffer(ISignaling signaling, DescData e)
        {
            var connectionId = e.connectionId;
            if (m_mapConnectionIdAndPeer.ContainsKey(connectionId))
            {
                Debug.LogError($"connection:{connectionId} peerConnection already exist");
                yield break;
            }

            var pc = CreatePeerConnection(signaling, connectionId, false);

            RTCSessionDescription _desc;
            _desc.type = RTCSdpType.Offer;
            _desc.sdp = e.sdp;

            var opRemoteDesc = pc.SetRemoteDescription(ref _desc);
            yield return opRemoteDesc;

            if (opRemoteDesc.IsError)
            {
                Debug.LogError($"Network Error: {opRemoteDesc.Error.message}");
                yield break;
            }

            // ToDo: need webrtc package version 2.3
            // foreach (var transceiver in pc.GetTransceivers()
            //     .Where(x => x.Receiver.Track.Kind == TrackKind.Video)
            //     .Select((x, index) => new {x, index})
            //     .Take(m_listVideoStreamTrack.Count))
            // {
            //     RTCRtpSender sender = transceiver.x.Sender;
            //     VideoStreamTrack track = m_listVideoStreamTrack[transceiver.index];
            //     transceiver.x.Sender.ReplaceTrack(track);
            //     transceiver.x.Direction = RTCRtpTransceiverDirection.SendOnly;
            //
            //     if (!m_mapTrackAndSenderList.TryGetValue(track, out List<RTCRtpSender> list))
            //     {
            //         list = new List<RTCRtpSender>();
            //         m_mapTrackAndSenderList.Add(track, list);
            //     }
            //
            //     list.Add(sender);
            // }

            foreach (var track in m_listVideoStreamTrack)
            {
                RTCRtpSender sender = pc.AddTrack(track);
                if (!m_mapTrackAndSenderList.TryGetValue(track, out List<RTCRtpSender> list))
                {
                    list = new List<RTCRtpSender>();
                    m_mapTrackAndSenderList.Add(track, list);
                }
                list.Add(sender);
            }

            foreach (var track in m_audioStream.GetTracks())
            {
                RTCRtpSender sender = pc.AddTrack(track);
                if (!m_mapTrackAndSenderList.TryGetValue(track, out List<RTCRtpSender> list))
                {
                    list = new List<RTCRtpSender>();
                    m_mapTrackAndSenderList.Add(track, list);
                }
                list.Add(sender);
            }

            RTCAnswerOptions options = default;
            var op = pc.CreateAnswer(ref options);
            yield return op;

            if (op.IsError)
            {
                Debug.LogError($"Network Error: {op.Error.message}");
                yield break;
            }

            var desc = op.Desc;
            var opLocalDesc = pc.SetLocalDescription(ref desc);
            yield return opLocalDesc;

            if (opLocalDesc.IsError)
            {
                Debug.LogError($"Network Error: {opLocalDesc.Error.message}");
                yield break;
            }

            signaling.SendAnswer(connectionId, desc);
        }

        RTCPeerConnection CreatePeerConnection(ISignaling signaling, string connectionId, bool isOffer)
        {
            if (m_mapConnectionIdAndPeer.TryGetValue(connectionId, out var peer))
            {
                peer.Close();
            }

            var pc = new RTCPeerConnection();
            m_mapConnectionIdAndPeer[connectionId] = pc;

            pc.OnDataChannel = new DelegateOnDataChannel(channel => { OnDataChannel(pc, channel); });
            pc.SetConfiguration(ref m_conf);
            pc.OnIceCandidate = new DelegateOnIceCandidate(candidate =>
            {
                signaling.SendCandidate(connectionId, candidate);
            });
            pc.OnIceConnectionChange = new DelegateOnIceConnectionChange(state =>
            {
                if(state == RTCIceConnectionState.Disconnected)
                {
                    pc.Close();
                    m_mapConnectionIdAndPeer.Remove(connectionId);
                }
            });

            pc.OnTrack = trackEvent =>
            {
                foreach (var receiveStream in m_listVideoReceiveStream)
                {
                    receiveStream.AddTrack(trackEvent.Track);
                }
            };

            pc.OnNegotiationNeeded = () => StartCoroutine(OnNegotiationNeeded(signaling, connectionId, isOffer));
            return pc;
        }

        IEnumerator OnNegotiationNeeded(ISignaling signaling, string connectionId, bool isOffer)
        {
            if (!isOffer)
            {
                yield break;
            }

            if (!m_mapConnectionIdAndPeer.TryGetValue(connectionId, out var pc))
            {
                Debug.LogError($"connectionId: {connectionId}, did not created peerConnection");
                yield break;
            }

            RTCOfferOptions option = new RTCOfferOptions
            {
                offerToReceiveAudio = true,
                offerToReceiveVideo = true
            };
            var offerOp = pc.CreateOffer(ref option);
            yield return offerOp;

            if (offerOp.IsError)
            {
                Debug.LogError($"Network Error: {offerOp.Error.message}");
                yield break;
            }

            if (pc.SignalingState != RTCSignalingState.Stable)
            {
                Debug.LogError($"peerConnection's signaling state is not stable.");
                yield break;
            }

            var desc = offerOp.Desc;
            var setLocalSdp = pc.SetLocalDescription(ref desc);
            yield return setLocalSdp;

            if (setLocalSdp.IsError)
            {
                Debug.LogError($"Network Error: {setLocalSdp.Error.message}");
                yield break;
            }

            signaling.SendOffer(connectionId, desc);
        }

        IEnumerator OnAnswer(ISignaling signaling, DescData e)
        {
            if (!m_mapConnectionIdAndPeer.TryGetValue(e.connectionId, out var pc))
            {
                Debug.Log($"connectiondId:{e.connectionId}, peerConnection not exist");
                yield break;
            }

            var desc = new RTCSessionDescription();
            desc.type = RTCSdpType.Answer;
            desc.sdp = e.sdp;
            var opRemoteSdp = pc.SetRemoteDescription(ref desc);
            yield return opRemoteSdp;

            if (opRemoteSdp.IsError)
            {
                Debug.LogError($"Network Error: {opRemoteSdp.Error.message}");
                yield break;
            }
        }

        void OnIceCandidate(ISignaling signaling, CandidateData e)
        {
            if (!m_mapConnectionIdAndPeer.TryGetValue(e.connectionId, out var pc))
            {
                return;
            }

            RTCIceCandidate _candidate = default;
            _candidate.candidate = e.candidate;
            _candidate.sdpMLineIndex = e.sdpMLineIndex;
            _candidate.sdpMid = e.sdpMid;

            pc.AddIceCandidate(ref _candidate);
        }

        void OnDataChannel(RTCPeerConnection pc, RTCDataChannel channel)
        {
            if (!m_mapPeerAndChannelDictionary.TryGetValue(pc, out var channels))
            {
                channels = new DataChannelDictionary();
                m_mapPeerAndChannelDictionary.Add(pc, channels);
            }
            channels.Add(channel.Id, channel);

            if (channel.Label != "data")
            {
                return;
            }

            RemoteInput input = RemoteInputReceiver.Create();
            input.ActionButtonClick = OnButtonClick;

            // device.current must be changed after creating devices
            m_defaultInput.MakeCurrent();

            m_mapChannelAndRemoteInput.Add(channel, input);
            channel.OnMessage = bytes => m_mapChannelAndRemoteInput[channel].ProcessInput(bytes);
            channel.OnClose = () => OnCloseChannel(channel);

            // find controller that not assigned remote input
            SimpleCameraController controller = m_listController
                .FirstOrDefault(_controller => !m_remoteInputAndCameraController.ContainsValue(_controller));

            if(controller != null)
            {
                controller.SetInput(input);
                m_remoteInputAndCameraController.Add(input, controller);

                byte index = (byte)m_listController.IndexOf(controller);
                byte[] bytes = {(byte)UnityEventType.SwitchVideo, index};
                channel.Send(bytes);
            }
        }

        void OnCloseChannel(RTCDataChannel channel)
        {
            RemoteInput input = m_mapChannelAndRemoteInput[channel];
            RemoteInputReceiver.Delete(input);

            // device.current must be changed after removing devices
            m_defaultInput.MakeCurrent();

            // reassign remote input to controller
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

            m_mapChannelAndRemoteInput.Remove(channel);
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
