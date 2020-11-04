using System;
using System.Collections;
using System.Collections.Generic;
using System.Threading;
using Unity.RenderStreaming.Signaling;
using Unity.WebRTC;
using UnityEngine;
using UnityEngine.UI;

namespace Unity.RenderStreaming
{
    public class OneOnOneSample : MonoBehaviour
    {
#pragma warning disable 0649
        [SerializeField] private Button startVideoButton;
        [SerializeField] private Button setUpButton;
        [SerializeField] private Button callButton;
        [SerializeField] private Button hangUpButton;
        [SerializeField] private RawImage localImage;
        [SerializeField] private RawImage remoteImage;
#pragma warning restore 0649

#pragma warning disable 0649
        [SerializeField, Tooltip("Signaling server url")]
        private string urlSignaling = "http://localhost";

        [SerializeField, Tooltip("Time interval for polling from signaling server")]
        private float interval = 5.0f;

        [SerializeField, Tooltip("Enable or disable hardware encoder")]
        private bool hardwareEncoderSupport = true;

#pragma warning restore 0649

        private SendRecvVideo m_sendRecv;

        private void Awake()
        {
            WebRTC.WebRTC.Initialize(hardwareEncoderSupport ? EncoderType.Hardware : EncoderType.Software);
            startVideoButton.onClick.AddListener(StartVideo);
            setUpButton.onClick.AddListener(SetUp);
            callButton.onClick.AddListener(Call);
            hangUpButton.onClick.AddListener(HangUp);
            m_sendRecv = new SendRecvVideo(urlSignaling, interval, SynchronizationContext.Current, this);
        }

        private void OnDestroy()
        {
            m_sendRecv?.Dispose();
            WebRTC.WebRTC.Dispose();
        }

        private void Start()
        {
            StartCoroutine(WebRTC.WebRTC.Update());
        }

        private void StartVideo()
        {
            StartCoroutine(m_sendRecv?.StartVideo(0, 1280, 720, localImage));
        }

        private void SetUp()
        {
            m_sendRecv?.SetUp(1280, 720, remoteImage);
        }

        private void Call()
        {
            m_sendRecv?.Call();
        }

        private void HangUp()
        {
            m_sendRecv?.HangUp();
            localImage.texture = null;
            remoteImage.texture = null;
        }
    }

    class SendRecvVideo : IDisposable
    {
        private readonly string m_signalingUrl;
        private readonly float m_timeout;
        private readonly SynchronizationContext m_context;
        private readonly MonoBehaviour m_parent;

        private static RTCConfiguration m_conf = new RTCConfiguration
        {
            iceServers = new[] {new RTCIceServer {urls = new[] {"stun:stun.l.google.com:19302"}}}
        };

        private Dictionary<string, RTCPeerConnection>
            m_mapConnectionIdAndPeer = new Dictionary<string, RTCPeerConnection>();

        private MediaStream m_localStream;
        private MediaStream m_remoteStream;
        private WebCamTexture m_webCamTexture;
        private ISignaling m_signaling;
        private string m_connectionId;

        public SendRecvVideo(string signalingUrl, float timeout, SynchronizationContext context, MonoBehaviour parent)
        {
            m_signalingUrl = signalingUrl;
            m_timeout = timeout;
            m_context = context;
            m_parent = parent;
        }

        public IEnumerator StartVideo(int deviceIndex, int width, int height, RawImage localImage)
        {
            if (WebCamTexture.devices.Length == 0)
            {
                Debug.LogFormat("WebCam device not found");
                yield break;
            }

            yield return Application.RequestUserAuthorization(UserAuthorization.WebCam);
            if (!Application.HasUserAuthorization(UserAuthorization.WebCam))
            {
                Debug.LogFormat("authorization for using the device is denied");
                yield break;
            }

            WebCamDevice userCameraDevice = WebCamTexture.devices[deviceIndex];
            m_webCamTexture = new WebCamTexture(userCameraDevice.name, width, height);
            m_webCamTexture.Play();
            yield return new WaitUntil(() => m_webCamTexture.didUpdateThisFrame);

            m_localStream = new MediaStream();
            m_localStream.AddTrack(new VideoStreamTrack(m_parent.name, m_webCamTexture));
            localImage.texture = m_webCamTexture;
        }

        public void SetUp(int width, int height, RawImage remoteImage)
        {
            m_remoteStream = new MediaStream();
            // ToDo: need update webrtc package to 2.2
            // m_remoteStream.OnAddTrack = e =>
            // {
            //     if (e.Track is VideoStreamTrack track)
            //     {
            //         remoteImage.texture = track.InitializeReceiver(width, height);
            //     }
            // };

            if (m_signalingUrl.StartsWith("ws") || m_signalingUrl.StartsWith("wss"))
            {
                m_signaling = new WebSocketSignaling(m_signalingUrl, m_timeout, m_context);
            }
            else
            {
                m_signaling = new HttpSignaling(m_signalingUrl, m_timeout, m_context);
            }

            m_signaling.OnStart += signaling => signaling.CreateConnection();
            m_signaling.OnCreateConnection += (signaling, id) =>
            {
                m_connectionId = id;
                CreatePeerConnection(signaling, m_connectionId, true);
            };
            m_signaling.OnOffer += (signaling, data) => m_parent.StartCoroutine(OnOffer(signaling, data));
            m_signaling.OnAnswer += (signaling, data) => m_parent.StartCoroutine(OnAnswer(signaling, data));
            m_signaling.OnIceCandidate += OnIceCandidate;

            m_signaling.Start();
        }

        public void Call()
        {
            if (!m_mapConnectionIdAndPeer.TryGetValue(m_connectionId, out var pc))
            {
                Debug.LogError($"peer connection is not prepared on connection id {m_connectionId}");
                return;
            }

            foreach (var track in m_localStream.GetTracks())
            {
                pc.AddTrack(track, m_localStream);
            }
        }

        public void HangUp()
        {
            foreach (var pc in m_mapConnectionIdAndPeer.Values)
            {
                if (pc.IceConnectionState != RTCIceConnectionState.Closed)
                {
                    pc.Close();
                }
            }

            m_remoteStream.Dispose();
            m_remoteStream = null;
            m_localStream.Dispose();
            m_localStream = null;
            m_signaling.Stop();
            m_signaling = null;

            m_mapConnectionIdAndPeer.Clear();
            m_webCamTexture.Stop();
            m_webCamTexture = null;
            m_connectionId = null;
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

            RTCSessionDescription offerDesc;
            offerDesc.type = RTCSdpType.Offer;
            offerDesc.sdp = e.sdp;

            var opRemoteDesc = pc.SetRemoteDescription(ref offerDesc);
            yield return opRemoteDesc;

            if (opRemoteDesc.IsError)
            {
                Debug.LogError($"Network Error: {opRemoteDesc.Error.message}");
                yield break;
            }

            foreach (var track in m_localStream.GetTracks())
            {
                pc.AddTrack(track, m_localStream);
            }

            RTCAnswerOptions options = default;
            var op = pc.CreateAnswer(ref options);
            yield return op;

            if (op.IsError)
            {
                Debug.LogError($"Network Error: {op.Error.message}");
                yield break;
            }

            var answerDesc = op.Desc;
            var opLocalDesc = pc.SetLocalDescription(ref answerDesc);
            yield return opLocalDesc;

            if (opLocalDesc.IsError)
            {
                Debug.LogError($"Network Error: {opLocalDesc.Error.message}");
                yield break;
            }

            signaling.SendAnswer(connectionId, answerDesc);
        }

        RTCPeerConnection CreatePeerConnection(ISignaling signaling, string connectionId, bool isOffer)
        {
            if (m_mapConnectionIdAndPeer.TryGetValue(connectionId, out var peer))
            {
                peer.Close();
            }

            var pc = new RTCPeerConnection(ref m_conf);
            m_mapConnectionIdAndPeer[connectionId] = pc;

            pc.OnIceCandidate = candidate => signaling.SendCandidate(connectionId, candidate);
            pc.OnIceConnectionChange = state =>
            {
                if (state != RTCIceConnectionState.Disconnected)
                {
                    return;
                }

                pc.Close();
                m_mapConnectionIdAndPeer.Remove(connectionId);
            };

            pc.OnTrack = trackEvent => m_remoteStream.AddTrack(trackEvent.Track);

            pc.OnNegotiationNeeded =
                () => m_parent.StartCoroutine(OnNegotiationNeeded(signaling, connectionId, isOffer));
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

            RTCOfferOptions option = new RTCOfferOptions {offerToReceiveAudio = true, offerToReceiveVideo = true};
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

        public void Dispose()
        {
            HangUp();
        }
    }
}
