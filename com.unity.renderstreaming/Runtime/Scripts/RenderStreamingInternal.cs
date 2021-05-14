using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using UnityEngine;
using Unity.RenderStreaming.Signaling;
using Unity.WebRTC;

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
        public event Action<string> onFoundConnection;

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
        private readonly Dictionary<string, RTCPeerConnection> _mapConnectionIdAndPeer =
            new Dictionary<string, RTCPeerConnection>();
        static List<RenderStreamingInternal> s_list = new List<RenderStreamingInternal>();

        /// <summary>
        ///
        /// </summary>
        /// <param name="dependencies"></param>
        public RenderStreamingInternal(ref RenderStreamingDependencies dependencies)
        {
            if(dependencies.signaling == null)
                throw new ArgumentException("Signaling instance is null.");
            if(dependencies.startCoroutine == null)
                throw new ArgumentException("Coroutine action instance is null.");

            if (s_list.Count == 0)
            {
                WebRTC.WebRTC.Initialize(dependencies.encoderType);
            }

            _config = dependencies.config;
            _startCoroutine = dependencies.startCoroutine;
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

            return peer.ConnectionState == RTCPeerConnectionState.Connected;
        }

        /// <summary>
        ///
        /// </summary>
        /// <param name="connectionId"></param>
        /// <param name="track"></param>
        public RTCRtpTransceiver AddTrack(string connectionId, MediaStreamTrack track)
        {
            RTCRtpSender sender = _mapConnectionIdAndPeer[connectionId].AddTrack(track);
            return _mapConnectionIdAndPeer[connectionId].GetTransceivers().First(t => t.Sender == sender);
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="connectionId"></param>
        /// <param name="kind"></param>
        /// <returns></returns>
        public RTCRtpTransceiver AddTrack(string connectionId, TrackKind kind)
        {
            return _mapConnectionIdAndPeer[connectionId].AddTransceiver(kind);
        }


        /// <summary>
        ///
        /// </summary>
        /// <param name="connectionId"></param>
        /// <param name="track"></param>
        public void RemoveTrack(string connectionId, MediaStreamTrack track)
        {
            var sender = GetSenders(connectionId).First(s => s.Track == track);
            _mapConnectionIdAndPeer[connectionId].RemoveTrack(sender);
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
            return _mapConnectionIdAndPeer[connectionId].CreateDataChannel(name, conf);
        }

        /// <summary>
        ///
        /// </summary>
        /// <param name="connectionId"></param>
        /// <param name="track"></param>
        /// <returns></returns>
        public IEnumerable<RTCRtpSender> GetSenders(string connectionId)
        {
            return _mapConnectionIdAndPeer[connectionId].GetSenders();
        }

        /// <summary>
        ///
        /// </summary>
        /// <param name="connectionId"></param>
        /// <param name="track"></param>
        /// <returns></returns>
        public IEnumerable<RTCRtpReceiver> GetReceivers(string connectionId)
        {
            return _mapConnectionIdAndPeer[connectionId].GetReceivers();
        }

        /// <summary>
        ///
        /// </summary>
        /// <param name="connectionId"></param>
        public void SendOffer(string connectionId)
        {
            _startCoroutine(SendOfferCoroutine(connectionId, _mapConnectionIdAndPeer[connectionId]));
        }

        /// <summary>
        ///
        /// </summary>
        /// <param name="connectionId"></param>
        public void SendAnswer(string connectionId)
        {
            _startCoroutine(SendAnswerCoroutine(connectionId, _mapConnectionIdAndPeer[connectionId]));
        }

        void OnStart(ISignaling signaling)
        {
            onStart?.Invoke();
        }

        void OnCreateConnection(ISignaling signaling, string connectionId, bool peerExists)
        {
            CreatePeerConnection(connectionId, peerExists);

            if(peerExists)
                onFoundConnection?.Invoke(connectionId);
            else
                onCreatedConnection?.Invoke(connectionId);
        }

        void OnDestroyConnection(ISignaling signaling, string connectionId)
        {
            DeletePeerConnection(connectionId);
            onDeletedConnection?.Invoke(connectionId);
        }

        RTCPeerConnection CreatePeerConnection(string connectionId, bool isOffer)
        {
            if (_mapConnectionIdAndPeer.TryGetValue(connectionId, out var peer))
            {
                peer.Close();
            }
            var pc = new RTCPeerConnection();
            _mapConnectionIdAndPeer[connectionId] = pc;

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
            pc.OnNegotiationNeeded = () => OnNegotiationNeeded(connectionId, isOffer);
            return pc;
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

        void OnNegotiationNeeded(string connectionId, bool isOffer)
        {
            if (!isOffer)
            {
                return;
            }
            SendOffer(connectionId);
        }

        IEnumerator SendOfferCoroutine(string connectionId, RTCPeerConnection pc)
        {
            var offerOp = pc.CreateOffer();
            yield return offerOp;

            if (offerOp.IsError)
            {
                Debug.LogError($"Network Error: {offerOp.Error.message}");
                yield break;
            }

            if (pc.SignalingState != RTCSignalingState.Stable)
            {
                Debug.LogError($"peerConnection's signaling state is not stable. {pc.SignalingState}");
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
            _signaling.SendOffer(connectionId, desc);
        }

        void OnAnswer(ISignaling signaling, DescData e)
        {
            if (!_mapConnectionIdAndPeer.TryGetValue(e.connectionId, out var pc))
            {
                Debug.Log($"connectionId:{e.connectionId}, peerConnection not exist");
                return;
            }
            _startCoroutine(GotAnswerCoroutine(e.connectionId, pc, e.sdp));
        }

        IEnumerator GotAnswerCoroutine(string connectionId, RTCPeerConnection pc, string sdp)
        {
            var desc = new RTCSessionDescription();
            desc.type = RTCSdpType.Answer;
            desc.sdp = sdp;
            var opRemoteSdp = pc.SetRemoteDescription(ref desc);
            yield return opRemoteSdp;

            if (opRemoteSdp.IsError)
            {
                Debug.LogError($"Network Error: {opRemoteSdp.Error.message}");
            }
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
                candidate = e.candidate,
                sdpMLineIndex = e.sdpMLineIndex,
                sdpMid = e.sdpMid
            };
            pc.AddIceCandidate(new RTCIceCandidate(option));
        }

        void OnOffer(ISignaling signaling, DescData e)
        {
            var connectionId = e.connectionId;
            if (!_mapConnectionIdAndPeer.TryGetValue(connectionId, out var pc))
            {
                pc = CreatePeerConnection(connectionId, false);
            }
            _startCoroutine(GotOfferCoroutine(connectionId, pc, e.sdp));
        }

        IEnumerator GotOfferCoroutine(string connectionId, RTCPeerConnection pc, string sdp)
        {
            RTCSessionDescription _desc;
            _desc.type = RTCSdpType.Offer;
            _desc.sdp = sdp;

            var opRemoteDesc = pc.SetRemoteDescription(ref _desc);
            yield return opRemoteDesc;

            if (opRemoteDesc.IsError)
            {
                Debug.LogError($"Network Error: {opRemoteDesc.Error.message}");
                yield break;
            }
            onGotOffer?.Invoke(connectionId, sdp);
        }

        IEnumerator SendAnswerCoroutine(string connectionId, RTCPeerConnection pc)
        {
            var op = pc.CreateAnswer();
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
            _signaling.SendAnswer(connectionId, desc);
        }
    }
}
