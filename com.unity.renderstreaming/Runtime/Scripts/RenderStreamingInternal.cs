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
    internal class RenderStreamingInternal : IDisposable
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
        private readonly List<MediaStreamTrack> m_listTrackForPublicMode =
            new List<MediaStreamTrack>();
        private readonly Dictionary<string, RTCPeerConnection> m_mapConnectionIdAndPeer =
            new Dictionary<string, RTCPeerConnection>();
        private readonly Dictionary<string, RTCDataChannel> m_mapConnectionIdAndChannel =
            new Dictionary<string, RTCDataChannel>();

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
        public void OpenConnection(string connectionId)
        {
            if (string.IsNullOrEmpty(connectionId))
                throw new ArgumentException("Argument of connectionId is null or empty.");
            _signaling.OpenConnection(connectionId);
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="connectionId"></param>
        public void CloseConnection(string connectionId)
        {
            if (string.IsNullOrEmpty(connectionId))
                throw new ArgumentException("Argument of connectionId is null or empty.");
            _signaling.CloseConnection(connectionId);
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="connectionId"></param>
        /// <param name="track"></param>
        public void AddTrack(string connectionId, MediaStreamTrack track)
        {
            if (string.IsNullOrEmpty(connectionId))
                throw new ArgumentException("Argument of connectionId is null or empty.");
            if (track == null)
                throw new ArgumentException("Argument of track is null or empty.");

            var pc = m_mapConnectionIdAndPeer[connectionId];
            if (pc.SignalingState != RTCSignalingState.Stable)
            {
                // todo:: RestartICE
                throw new InvalidOperationException($"peerConnection's signaling state is not stable. {pc.SignalingState}");
            }
            pc.AddTrack(track);
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="connectionId"></param>
        /// <param name="track"></param>
        public void RemoveTrack(string connectionId, MediaStreamTrack track)
        {
            if (string.IsNullOrEmpty(connectionId))
                throw new ArgumentException("Argument of connectionId is null or empty.");
            if (track == null)
                throw new ArgumentException("Argument of track is null or empty.");

            var sender = GetSenders(connectionId, track).First(s => s.Track == track);
            m_mapConnectionIdAndPeer[connectionId].RemoveTrack(sender);
        }

        /// <summary>
        /// for public mode.
        /// </summary>
        /// <param name="track"></param>
        public void AddTrack(MediaStreamTrack track)
        {
            m_listTrackForPublicMode.Add(track);
        }

        /// <summary>
        /// for public mode.
        /// </summary>
        /// <param name="track"></param>
        public void RemoveTrack(MediaStreamTrack track)
        {
            m_listTrackForPublicMode.Remove(track);
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="connectionId"></param>
        /// <param name="name"></param>
        /// <returns></returns>
        public RTCDataChannel CreateChannel(string connectionId, string name)
        {
            if (string.IsNullOrEmpty(connectionId))
                throw new ArgumentException("Arguments of connectionId is null or empty.");

            var pc = m_mapConnectionIdAndPeer[connectionId];
            if (pc.SignalingState != RTCSignalingState.Stable)
            {
                // todo:: RestartICE
                throw new InvalidOperationException($"peerConnection's signaling state is not stable. {pc.SignalingState}");
            }
            RTCDataChannelInit conf = new RTCDataChannelInit();
            return m_mapConnectionIdAndPeer[connectionId].CreateDataChannel(name, conf);
        }

        public IEnumerable<RTCRtpSender> GetSenders(string connectionId, MediaStreamTrack track)
        {
            return m_mapConnectionIdAndPeer[connectionId].GetSenders();
        }

        public IEnumerable<RTCRtpReceiver> GetReceivers(string connectionId, MediaStreamTrack track)
        {
            return m_mapConnectionIdAndPeer[connectionId].GetReceivers();
        }

        void OnStart(ISignaling signaling)
        {
            // this connection is for public mode
            //signaling.OpenConnection(Guid.NewGuid().ToString());
            onStart?.Invoke();
        }

        void OnCreateConnection(ISignaling signaling, string connectionId, bool peerExists)
        {
            CreatePeerConnection(connectionId, peerExists);

            onCreatedConnection?.Invoke(connectionId);
        }

        void OnDestroyConnection(ISignaling signaling, string connectionId)
        {
            DeletePeerConnection(connectionId);

            onDeletedConnection?.Invoke(connectionId);
        }

        RTCPeerConnection CreatePeerConnection(string connectionId, bool isOffer)
        {
            if (m_mapConnectionIdAndPeer.TryGetValue(connectionId, out var peer))
            {
                peer.Close();
            }
            var pc = new RTCPeerConnection();
            m_mapConnectionIdAndPeer[connectionId] = pc;

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
            if (m_mapConnectionIdAndPeer.TryGetValue(connectionId, out var pc))
            {
                //RemoveTracks(connectionId, pc);
                // m_mapPeerAndChannelDictionary.Remove(pc);
                pc.Dispose();
            }
            m_mapConnectionIdAndPeer.Remove(connectionId);
        }

        void OnDataChannel(string connectionId, RTCDataChannel channel)
        {
            m_mapConnectionIdAndChannel.Add(connectionId, channel);
            onAddChannel?.Invoke(connectionId, channel);

            //if (!m_mapPeerAndChannelDictionary.TryGetValue(pc, out var channels))
            //{
            //    channels = new DataChannelDictionary();
            //    m_mapPeerAndChannelDictionary.Add(pc, channels);
            //}

            //channels.Add(channel.Id, channel);

            //if (channel.Label != "data")
            //{
            //    return;
            //}

            //RemoteInput input = RemoteInputReceiver.Create();
            //input.ActionButtonClick = OnButtonClick;

            //// device.current must be changed after creating devices
            //m_defaultInput.MakeCurrent();

            //m_mapChannelAndRemoteInput.Add(channel, input);
            //channel.OnMessage = bytes => m_mapChannelAndRemoteInput[channel].ProcessInput(bytes);
            //channel.OnClose = () => OnCloseChannel(channel);

            //// find controller that not assigned remote input
            //SimpleCameraController controller = m_listController
            //    .FirstOrDefault(_controller => !m_remoteInputAndCameraController.ContainsValue(_controller));

            //if (controller != null)
            //{
            //    controller.SetInput(input);
            //    m_remoteInputAndCameraController.Add(input, controller);

            //    byte index = (byte)m_listController.IndexOf(controller);
            //    byte[] bytes = { (byte)UnityEventType.SwitchVideo, index };
            //    channel.Send(bytes);
            //}
        }

        void OnIceConnectionChange(string connectionId, RTCIceConnectionState state)
        {
            switch (state)
            {
                case RTCIceConnectionState.Connected:
                    onConnect?.Invoke(connectionId);
                    break;
                case RTCIceConnectionState.Disconnected:
                    m_mapConnectionIdAndPeer[connectionId].Close();
                    m_mapConnectionIdAndPeer.Remove(connectionId);
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
            _startCoroutine(SendOffer(connectionId));
        }

        IEnumerator SendOffer(string connectionId)
        {
            if (!m_mapConnectionIdAndPeer.TryGetValue(connectionId, out var pc))
            {
                Debug.LogError($"connectionId: {connectionId}, did not created peerConnection");
                yield break;
            }
            RTCOfferOptions option = new RTCOfferOptions { offerToReceiveAudio = true, offerToReceiveVideo = true };
            var offerOp = pc.CreateOffer(ref option);
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

        void OnAnswer(ISignaling m_signaling, DescData e)
        {
            _startCoroutine(OnAnswerCoroutine(e));
        }

        IEnumerator OnAnswerCoroutine(DescData e)
        {
            if (!m_mapConnectionIdAndPeer.TryGetValue(e.connectionId, out var pc))
            {
                Debug.Log($"connectionId:{e.connectionId}, peerConnection not exist");
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

        void OnIceCandidate(ISignaling m_signaling, CandidateData e)
        {
            if (!m_mapConnectionIdAndPeer.TryGetValue(e.connectionId, out var pc))
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
            if (!m_mapConnectionIdAndPeer.TryGetValue(connectionId, out var pc))
            {
                pc = CreatePeerConnection(connectionId, false);
            }

            _startCoroutine(SendAnswerCoroutine(connectionId, pc, e.sdp));
        }

        IEnumerator SendAnswerCoroutine(string connectionId, RTCPeerConnection pc, string sdp)
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

            // TODO:: make callback
            //foreach (var track in m_listTrackForPublicMode)
            //{
            //    Debug.Log("track");
            //    pc.AddTrack(track);
            //}


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
            _signaling.SendAnswer(connectionId, desc);
        }
    }
}
