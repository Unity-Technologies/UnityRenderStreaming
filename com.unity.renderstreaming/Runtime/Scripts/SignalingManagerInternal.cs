using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using Unity.RenderStreaming.Signaling;
using Unity.WebRTC;
using UnityEngine;

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
        public RTCConfiguration config;

        /// <summary>
        ///
        /// </summary>
        public Func<IEnumerator, Coroutine> startCoroutine;

        /// <summary>
        ///
        /// </summary>
        public Action<Coroutine> stopCoroutine;

        /// <summary>
        /// unit is second;
        /// </summary>
        public float resentOfferInterval;
    }

    /// <summary>
    ///
    /// </summary>
    internal class SignalingManagerInternal : IDisposable,
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
        public event Action<string, RTCRtpTransceiver> onAddTransceiver;

        /// <summary>
        ///
        /// </summary>
        public event Action<string, RTCDataChannel> onAddChannel;

        private bool _disposed;
        private readonly ISignaling _signaling;
        private RTCConfiguration _config;
        private readonly Func<IEnumerator, Coroutine> _startCoroutine;
        private readonly Action<Coroutine> _stopCoroutine;
        private readonly Dictionary<string, PeerConnection> _mapConnectionIdAndPeer =
            new Dictionary<string, PeerConnection>();
        private bool _runningResendCoroutine;
        private float _resendInterval = 3.0f;

        /// <summary>
        ///
        /// </summary>
        /// <param name="dependencies"></param>
        public SignalingManagerInternal(ref RenderStreamingDependencies dependencies)
        {
            if (dependencies.signaling == null)
                throw new ArgumentException("Signaling instance is null.");
            if (dependencies.startCoroutine == null)
                throw new ArgumentException("Coroutine action instance is null.");

            _config = dependencies.config;
            _startCoroutine = dependencies.startCoroutine;
            _stopCoroutine = dependencies.stopCoroutine;
            _resendInterval = dependencies.resentOfferInterval;
            _signaling = dependencies.signaling;
            _signaling.OnStart += OnStart;
            _signaling.OnCreateConnection += OnCreateConnection;
            _signaling.OnDestroyConnection += OnDestroyConnection;
            _signaling.OnOffer += OnOffer;
            _signaling.OnAnswer += OnAnswer;
            _signaling.OnIceCandidate += OnIceCandidate;
            _signaling.Start();
            _startCoroutine(WebRTC.WebRTC.Update());
        }

        /// <summary>
        ///
        /// </summary>
        ~SignalingManagerInternal()
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

            foreach (var pair in _mapConnectionIdAndPeer)
                pair.Value.Dispose();

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
            return _mapConnectionIdAndPeer.TryGetValue(connectionId, out var peer) && peer.IsConnected();
        }

        public bool IsStable(string connectionId)
        {
            return _mapConnectionIdAndPeer.TryGetValue(connectionId, out var peer) && peer.IsStable();
        }

        /// <summary>
        ///
        /// </summary>
        /// <param name="connectionId"></param>
        /// <param name="track"></param>
        public void RemoveSenderTrack(string connectionId, MediaStreamTrack track)
        {
            var sender = GetSenders(connectionId).First(s => s.Track == track);
            _mapConnectionIdAndPeer[connectionId].peer.RemoveTrack(sender);
        }

        /// <summary>
        ///
        /// </summary>
        /// <param name="connectionId"></param>
        /// <param name="track"></param>
        /// <param name="direction"></param>
        /// <returns></returns>
        public RTCRtpTransceiver AddTransceiver(string connectionId, MediaStreamTrack track, RTCRtpTransceiverInit init = null)
        {
            var transceiver = _mapConnectionIdAndPeer[connectionId].peer.AddTransceiver(track, init);
            return transceiver;
        }

        /// <summary>
        ///
        /// </summary>
        /// <param name="connectionId"></param>
        /// <param name="kind"></param>
        /// <param name="direction"></param>
        /// <returns></returns>
        public RTCRtpTransceiver AddTransceiver(string connectionId, TrackKind kind, RTCRtpTransceiverInit init = null)
        {
            var transceiver = _mapConnectionIdAndPeer[connectionId].peer.AddTransceiver(kind, init);
            return transceiver;
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
            if (string.IsNullOrEmpty(name))
                name = Guid.NewGuid().ToString();
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
        /// <param name="track"></param>
        /// <returns></returns>
        public IEnumerable<RTCRtpTransceiver> GetTransceivers(string connectionId)
        {
            return _mapConnectionIdAndPeer[connectionId].peer.GetTransceivers();
        }

        /// <summary>
        ///
        /// </summary>
        /// <param name="connectionId"></param>
        public void SendOffer(string connectionId)
        {
            if (!_mapConnectionIdAndPeer.TryGetValue(connectionId, out var pc))
                return;
            pc.SendOffer();
        }

        /// <summary>
        ///
        /// </summary>
        /// <param name="connectionId"></param>
        public void SendAnswer(string connectionId)
        {
            if (!_mapConnectionIdAndPeer.TryGetValue(connectionId, out var pc))
                return;
            pc.SendAnswer();
        }

        IEnumerator ResendOfferCoroutine()
        {
            HashSet<string> failedConnections = new HashSet<string>();
            while (_runningResendCoroutine)
            {
                failedConnections.Clear();
                foreach (var peer in _mapConnectionIdAndPeer)
                {
                    if (peer.Value.peer.ConnectionState == RTCPeerConnectionState.Failed)
                    {
                        failedConnections.Add(peer.Key);
                    }
                    else if (peer.Value.waitingAnswer)
                    {
                        peer.Value.SendOffer();
                    }
                }

                foreach (var connectionId in failedConnections)
                {
                    DestroyConnection(connectionId);
                }

                yield return 0;
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
            DestroyConnection(connectionId);
        }

        void DestroyConnection(string connectionId)
        {
            DeletePeerConnection(connectionId);
            onDeletedConnection?.Invoke(connectionId);
        }

        PeerConnection CreatePeerConnection(string connectionId, bool polite)
        {
            if (_mapConnectionIdAndPeer.TryGetValue(connectionId, out var peer))
            {
                peer.Dispose();
            }

            peer = new PeerConnection(polite, _config, _resendInterval, _startCoroutine, _stopCoroutine);
            _mapConnectionIdAndPeer[connectionId] = peer;

            peer.OnConnectHandler += () => onConnect?.Invoke(connectionId);
            peer.OnDisconnectHandler += () => onDisconnect?.Invoke(connectionId);
            peer.OnDataChannelHandler += channel => onAddChannel?.Invoke(connectionId, channel); ;
            peer.OnTrackEventHandler += e => onAddTransceiver?.Invoke(connectionId, e.Transceiver);
            peer.SendOfferHandler += desc => _signaling?.SendOffer(connectionId, desc);
            peer.SendAnswerHandler += desc => _signaling?.SendAnswer(connectionId, desc);
            peer.SendCandidateHandler += candidate => _signaling?.SendCandidate(connectionId, candidate);
            return peer;
        }

        void DeletePeerConnection(string connectionId)
        {
            if (!_mapConnectionIdAndPeer.TryGetValue(connectionId, out var peer))
            {
                return;
            }

            peer.Dispose();
            _mapConnectionIdAndPeer.Remove(connectionId);
        }

        void OnAnswer(ISignaling signaling, DescData e)
        {
            if (!_mapConnectionIdAndPeer.TryGetValue(e.connectionId, out var pc))
            {
                RenderStreaming.Logger.Log(LogType.Warning, $"connectionId:{e.connectionId}, peerConnection not exist");
                return;
            }

            RTCSessionDescription description = new RTCSessionDescription { type = RTCSdpType.Answer, sdp = e.sdp };
            _startCoroutine(pc.OnGotDescription(description, () => onGotAnswer?.Invoke(e.connectionId, e.sdp)));
        }

        void OnIceCandidate(ISignaling signaling, CandidateData e)
        {
            if (!_mapConnectionIdAndPeer.TryGetValue(e.connectionId, out var pc))
            {
                return;
            }

            RTCIceCandidateInit option = new RTCIceCandidateInit
            {
                candidate = e.candidate,
                sdpMLineIndex = e.sdpMLineIndex,
                sdpMid = e.sdpMid
            };
            pc.OnGotIceCandidate(new RTCIceCandidate(option));
        }

        void OnOffer(ISignaling signaling, DescData e)
        {
            var connectionId = e.connectionId;
            if (!_mapConnectionIdAndPeer.TryGetValue(connectionId, out var pc))
            {
                pc = CreatePeerConnection(connectionId, e.polite);
            }

            RTCSessionDescription description = new RTCSessionDescription { type = RTCSdpType.Offer, sdp = e.sdp };
            _startCoroutine(pc.OnGotDescription(description, () => onGotOffer?.Invoke(connectionId, e.sdp)));
        }
    }
}
