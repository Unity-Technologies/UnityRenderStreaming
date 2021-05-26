using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using UnityEngine;
using Unity.RenderStreaming.Signaling;
using Unity.WebRTC;
using UnityEngine.Assertions;

namespace Unity.RenderStreaming
{
    /// <summary>
    ///
    /// </summary>
    internal struct RenderStreamingDependencies
    {
        /// <summary>
        ///
        /// </summary>
        public ISignaling signaling;

        /// <summary>
        ///
        /// </summary>
        public EncoderType encoderType;

        /// <summary>
        ///
        /// </summary>
        public RTCConfiguration config;

        /// <summary>
        ///
        /// </summary>
        public Func<IEnumerator, Coroutine> startCoroutine;

        /// <summary>
        /// unit is second;
        /// </summary>
        public float resentOfferInterval;
    }

    /// <summary>
    ///
    /// </summary>
    internal class RenderStreamingInternal : IDisposable,
        IRenderStreamingHandler, IRenderStreamingDelegate
    {
        /// <summary>
        ///
        /// </summary>
        public event Action onStart;

        /// <summary>
        ///
        /// </summary>
        public event Action<string> onCreatedConnection;

        /// <summary>
        ///
        /// </summary>
        public event Action<string> onDeletedConnection;

        /// <summary>
        ///
        /// </summary>
        public event Action<string, string> onGotOffer;

        /// <summary>
        ///
        /// </summary>
        public event Action<string, string> onGotAnswer;

        /// <summary>
        ///
        /// </summary>
        public event Action<string> onConnect;

        /// <summary>
        ///
        /// </summary>
        public event Action<string> onDisconnect;

        /// <summary>
        ///
        /// </summary>
        public event Action<string, RTCRtpReceiver> onAddReceiver;

        /// <summary>
        ///
        /// </summary>
        public event Action<string, RTCDataChannel> onAddChannel;

        private bool _disposed;
        private readonly ISignaling _signaling;
        private RTCConfiguration _config;
        private readonly Func<IEnumerator, Coroutine> _startCoroutine;
        private readonly Dictionary<string, PeerConnection> _mapConnectionIdAndPeer =
            new Dictionary<string, PeerConnection>();
        private bool _runningResendCoroutine;
        private float _resendInterval = 1.0f;

        static List<RenderStreamingInternal> s_list = new List<RenderStreamingInternal>();

        /// <summary>
        ///
        /// </summary>
        /// <param name="dependencies"></param>
        public RenderStreamingInternal(ref RenderStreamingDependencies dependencies)
        {
            if (dependencies.signaling == null)
                throw new ArgumentException("Signaling instance is null.");
            if (dependencies.startCoroutine == null)
                throw new ArgumentException("Coroutine action instance is null.");

            if (s_list.Count == 0)
            {
                WebRTC.WebRTC.Initialize(dependencies.encoderType);
            }

            _config = dependencies.config;
            _startCoroutine = dependencies.startCoroutine;
            _resendInterval = dependencies.resentOfferInterval;
            _signaling = dependencies.signaling;
            _signaling.OnStart += OnStart;
            _signaling.OnCreateConnection += OnCreateConnection;
            _signaling.OnDestroyConnection += OnDestroyConnection;
            _signaling.OnOffer += OnOffer;
            _signaling.OnAnswer += OnAnswer;
            _signaling.OnIceCandidate += OnIceCandidate;
            _signaling.Start();

            s_list.Add(this);
            _startCoroutine(WebRTC.WebRTC.Update());
        }

        /// <summary>
        ///
        /// </summary>
        ~RenderStreamingInternal()
        {
            Dispose();
        }

        /// <summary>
        ///
        /// </summary>
        public void Dispose()
        {
            if (this._disposed)
            {
                return;
            }

            _runningResendCoroutine = false;

            _signaling.Stop();
            _signaling.OnStart -= OnStart;
            _signaling.OnCreateConnection -= OnCreateConnection;
            _signaling.OnDestroyConnection -= OnDestroyConnection;
            _signaling.OnOffer -= OnOffer;
            _signaling.OnAnswer -= OnAnswer;
            _signaling.OnIceCandidate -= OnIceCandidate;

            s_list.Remove(this);
            if (s_list.Count == 0)
            {
                WebRTC.WebRTC.Dispose();
            }

            this._disposed = true;
            GC.SuppressFinalize(this);
        }

        /// <summary>
        ///
        /// </summary>
        /// <param name="connectionId"></param>
        public void CreateConnection(string connectionId)
        {
            _signaling.OpenConnection(connectionId);
        }

        /// <summary>
        ///
        /// </summary>
        /// <param name="connectionId"></param>
        public void DeleteConnection(string connectionId)
        {
            _signaling.CloseConnection(connectionId);
        }

        public bool ExistConnection(string connectionId)
        {
            return _mapConnectionIdAndPeer.ContainsKey(connectionId);
        }

        public bool IsConnected(string connectionId)
        {
            if (!_mapConnectionIdAndPeer.TryGetValue(connectionId, out var peer))
                return false;

            return peer.peer.ConnectionState == RTCPeerConnectionState.Connected;
        }

        public bool IsStable(string connectionId)
        {
            if (!_mapConnectionIdAndPeer.TryGetValue(connectionId, out var peer))
                return false;

            if (peer.makingOffer || peer.srdAnswerPending || peer.waitingAnswer)
            {
                return false;
            }

            return peer.peer.SignalingState == RTCSignalingState.Stable;
        }

        /// <summary>
        ///
        /// </summary>
        /// <param name="connectionId"></param>
        /// <param name="track"></param>
        public RTCRtpTransceiver AddTrack(string connectionId, MediaStreamTrack track)
        {
            RTCRtpSender sender = _mapConnectionIdAndPeer[connectionId].peer.AddTrack(track);
            return _mapConnectionIdAndPeer[connectionId].peer.GetTransceivers().First(t => t.Sender == sender);
        }

        /// <summary>
        ///
        /// </summary>
        /// <param name="connectionId"></param>
        /// <param name="kind"></param>
        /// <returns></returns>
        public RTCRtpTransceiver AddTrack(string connectionId, TrackKind kind)
        {
            return _mapConnectionIdAndPeer[connectionId].peer.AddTransceiver(kind);
        }


        /// <summary>
        ///
        /// </summary>
        /// <param name="connectionId"></param>
        /// <param name="track"></param>
        public void RemoveTrack(string connectionId, MediaStreamTrack track)
        {
            var sender = GetSenders(connectionId).First(s => s.Track == track);
            _mapConnectionIdAndPeer[connectionId].peer.RemoveTrack(sender);
        }

        /// <summary>
        ///
        /// </summary>
        /// <param name="connectionId"></param>
        /// <param name="name"></param>
        /// <returns></returns>
        public RTCDataChannel CreateChannel(string connectionId, string name)
        {
            RTCDataChannelInit conf = new RTCDataChannelInit();
            return _mapConnectionIdAndPeer[connectionId].peer.CreateDataChannel(name, conf);
        }

        /// <summary>
        ///
        /// </summary>
        /// <param name="connectionId"></param>
        /// <param name="track"></param>
        /// <returns></returns>
        public IEnumerable<RTCRtpSender> GetSenders(string connectionId)
        {
            return _mapConnectionIdAndPeer[connectionId].peer.GetSenders();
        }

        /// <summary>
        ///
        /// </summary>
        /// <param name="connectionId"></param>
        /// <param name="track"></param>
        /// <returns></returns>
        public IEnumerable<RTCRtpReceiver> GetReceivers(string connectionId)
        {
            return _mapConnectionIdAndPeer[connectionId].peer.GetReceivers();
        }

        /// <summary>
        ///
        /// </summary>
        /// <param name="connectionId"></param>
        public void SendOffer(string connectionId)
        {
            var pc = _mapConnectionIdAndPeer[connectionId];
            if (!IsStable(connectionId))
            {
                if (!pc.waitingAnswer)
                {
                    throw new InvalidOperationException(
                        $"{pc} sendoffer needs in stable state, current state is {pc.peer.SignalingState}");
                }

                Debug.Log($"re-sent offer sdp:{pc.peer.LocalDescription.sdp}");
                _signaling.SendOffer(connectionId, pc.peer.LocalDescription);
                return;
            }

            _startCoroutine(SendOfferCoroutine(connectionId, pc));
        }

        /// <summary>
        ///
        /// </summary>
        /// <param name="connectionId"></param>
        public void SendAnswer(string connectionId)
        {
            _startCoroutine(SendAnswerCoroutine(connectionId, _mapConnectionIdAndPeer[connectionId]));
        }

        IEnumerator ResendOfferCoroutine()
        {
            while (_runningResendCoroutine)
            {
                foreach (var pair in _mapConnectionIdAndPeer.Where(x => x.Value.waitingAnswer))
                {
                    _signaling.SendOffer(pair.Key, pair.Value.peer.LocalDescription);
                }

                yield return new WaitForSecondsRealtime((int)(_resendInterval * 1000));
            }
        }

        void OnStart(ISignaling signaling)
        {
            if (!_runningResendCoroutine)
            {
                _runningResendCoroutine = true;
                _startCoroutine(ResendOfferCoroutine());
            }
            onStart?.Invoke();
        }

        void OnCreateConnection(ISignaling signaling, string connectionId, bool polite)
        {
            CreatePeerConnection(connectionId, polite);
            onCreatedConnection?.Invoke(connectionId);
        }

        void OnDestroyConnection(ISignaling signaling, string connectionId)
        {
            DeletePeerConnection(connectionId);
            onDeletedConnection?.Invoke(connectionId);
        }

        PeerConnection CreatePeerConnection(string connectionId, bool polite)
        {
            if (_mapConnectionIdAndPeer.TryGetValue(connectionId, out var peer))
            {
                peer.peer.Close();
            }

            var pc = new RTCPeerConnection();
            peer = new PeerConnection(pc, polite);
            _mapConnectionIdAndPeer[connectionId] = peer;

            pc.SetConfiguration(ref _config);
            pc.OnDataChannel = channel => { OnDataChannel(connectionId, channel); };
            pc.OnIceCandidate = candidate =>
            {
                _signaling.SendCandidate(connectionId, candidate);
            };
            pc.OnIceConnectionChange = state => OnIceConnectionChange(connectionId, state);

            pc.OnTrack = trackEvent =>
            {
                onAddReceiver?.Invoke(connectionId, trackEvent.Receiver);
            };
            pc.OnNegotiationNeeded = () => OnNegotiationNeeded(connectionId);
            return peer;
        }

        void DeletePeerConnection(string connectionId)
        {
            _mapConnectionIdAndPeer[connectionId].Dispose();
            _mapConnectionIdAndPeer.Remove(connectionId);
        }

        void OnDataChannel(string connectionId, RTCDataChannel channel)
        {
            onAddChannel?.Invoke(connectionId, channel);
        }

        void OnIceConnectionChange(string connectionId, RTCIceConnectionState state)
        {
            switch (state)
            {
                case RTCIceConnectionState.Connected:
                    onConnect?.Invoke(connectionId);
                    break;
                case RTCIceConnectionState.Disconnected:
                    onDisconnect?.Invoke(connectionId);
                    break;
            }
        }

        void OnNegotiationNeeded(string connectionId)
        {
            SendOffer(connectionId);
        }

        IEnumerator SendOfferCoroutine(string connectionId, PeerConnection pc)
        {
            Debug.Log($"{pc} SLD due to negotiationneeded");
            yield return new WaitWhile(() => pc.sldGetBackStable);

            Assert.AreEqual(pc.peer.SignalingState, RTCSignalingState.Stable,
                $"{pc} negotiationneeded always fires in stable state");
            Assert.AreEqual(pc.makingOffer, false, $"{pc} negotiationneeded not already in progress");

            pc.makingOffer = true;
            var opLocalDesc = pc.peer.SetLocalDescription();
            yield return opLocalDesc;

            if (opLocalDesc.IsError)
            {
                Debug.LogError($"{pc} {opLocalDesc.Error.message}");
                pc.makingOffer = false;
                yield break;
            }

            Assert.AreEqual(pc.peer.SignalingState, RTCSignalingState.HaveLocalOffer,
                $"{pc} negotiationneeded not racing with onmessage");
            Assert.AreEqual(pc.peer.LocalDescription.type, RTCSdpType.Offer, $"{pc} negotiationneeded SLD worked");
            pc.makingOffer = false;
            pc.waitingAnswer = true;

            _signaling.SendOffer(connectionId, pc.peer.LocalDescription);
        }

        void OnAnswer(ISignaling signaling, DescData e)
        {
            if (!_mapConnectionIdAndPeer.TryGetValue(e.connectionId, out var pc))
            {
                Debug.Log($"connectionId:{e.connectionId}, peerConnection not exist");
                return;
            }

            pc.waitingAnswer = false;
            _startCoroutine(GotAnswerCoroutine(e.connectionId, pc, e.sdp));
        }

        IEnumerator GotAnswerCoroutine(string connectionId, PeerConnection pc, string sdp)
        {
            var description = new RTCSessionDescription();
            description.type = RTCSdpType.Answer;
            description.sdp = sdp;

            yield return new WaitWhile(() => pc.makingOffer);

            pc.srdAnswerPending = true;

            Debug.Log($"{pc} SRD {description.type} SignalingState {pc.peer.SignalingState}");
            var opRemoteDesc = pc.peer.SetRemoteDescription(ref description);
            yield return opRemoteDesc;

            if (opRemoteDesc.IsError)
            {
                Debug.LogError($"{pc} {opRemoteDesc.Error.message}");
                pc.srdAnswerPending = false;
                yield break;
            }

            Assert.AreEqual(pc.peer.RemoteDescription.type, RTCSdpType.Answer, $"{pc} Answer was set");
            Assert.AreEqual(pc.peer.SignalingState, RTCSignalingState.Stable, $"{pc} answered");
            pc.srdAnswerPending = false;

            onGotAnswer?.Invoke(connectionId, sdp);
        }

        void OnIceCandidate(ISignaling signaling, CandidateData e)
        {
            if (!_mapConnectionIdAndPeer.TryGetValue(e.connectionId, out var pc))
            {
                return;
            }

            RTCIceCandidateInit option = new RTCIceCandidateInit
            {
                candidate = e.candidate, sdpMLineIndex = e.sdpMLineIndex, sdpMid = e.sdpMid
            };

            if (!pc.peer.AddIceCandidate(new RTCIceCandidate(option)) && !pc.ignoreOffer)
            {
                Debug.LogWarning($"{pc} this candidate can't accept current signaling state {pc.peer.SignalingState}.");
            }
        }

        void OnOffer(ISignaling signaling, DescData e)
        {
            var connectionId = e.connectionId;
            if (!_mapConnectionIdAndPeer.TryGetValue(connectionId, out var pc))
            {
                pc = CreatePeerConnection(connectionId, e.polite);
            }

            pc.waitingAnswer = false;
            _startCoroutine(GotOfferCoroutine(connectionId, pc, e.sdp));
        }

        IEnumerator GotOfferCoroutine(string connectionId, PeerConnection pc, string sdp)
        {
            RTCSessionDescription description;
            description.type = RTCSdpType.Offer;
            description.sdp = sdp;

            var isStable =
                pc.peer.SignalingState == RTCSignalingState.Stable ||
                (pc.peer.SignalingState == RTCSignalingState.HaveLocalOffer && pc.srdAnswerPending);
            pc.ignoreOffer =
                description.type == RTCSdpType.Offer && !pc.polite && (pc.makingOffer || !isStable);
            if (pc.ignoreOffer)
            {
                Debug.Log($"{pc} glare - ignoring offer");
                yield break;
            }

            yield return new WaitWhile(() => pc.makingOffer);

            Debug.Log($"{pc} SRD {description.type} SignalingState {pc.peer.SignalingState}");
            var opRemoteDesc = pc.peer.SetRemoteDescription(ref description);
            yield return opRemoteDesc;

            if (opRemoteDesc.IsError)
            {
                Debug.LogError($"{pc} {opRemoteDesc.Error.message}");
                yield break;
            }

            Assert.AreEqual(pc.peer.RemoteDescription.type, RTCSdpType.Offer, $"{pc} SRD worked");
            Assert.AreEqual(pc.peer.SignalingState, RTCSignalingState.HaveRemoteOffer, $"{pc} Remote offer");

            onGotOffer?.Invoke(connectionId, sdp);
        }

        IEnumerator SendAnswerCoroutine(string connectionId, PeerConnection pc)
        {
            Debug.Log($"{pc} SLD to get back to stable");
            pc.sldGetBackStable = true;

            var opLocalDesc = pc.peer.SetLocalDescription();
            yield return opLocalDesc;

            if (opLocalDesc.IsError)
            {
                Debug.LogError($"{pc} {opLocalDesc.Error.message}");
                pc.sldGetBackStable = false;
                yield break;
            }

            Assert.AreEqual(pc.peer.LocalDescription.type, RTCSdpType.Answer, $"{pc} onmessage SLD worked");
            Assert.AreEqual(pc.peer.SignalingState, RTCSignalingState.Stable,
                $"{pc} onmessage not racing with negotiationneeded");
            pc.sldGetBackStable = false;

            _signaling.SendAnswer(connectionId, pc.peer.LocalDescription);
        }
    }
}
