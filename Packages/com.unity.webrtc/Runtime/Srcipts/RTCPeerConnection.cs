using UnityEngine;
using System.Runtime.InteropServices;
using System;

namespace Unity.WebRTC
{
    public class RTCPeerConnection : IDisposable
    {
        private int m_id;
        private IntPtr self = IntPtr.Zero;
        private DelegateOnIceConnectionChange onIceConnectionChange;
        private DelegateOnIceConnectionChange selfOnIceConnectionChange;
        private DelegateOnIceCandidate onIceCandidate;
        private DelegateNativeOnIceCandidate selfOnIceCandidate;
        private DelegateOnDataChannel onDataChannel;
        private DelegateNativeOnDataChannel selfOnDataChannel;
        private DelegateNativeOnTrack selfOnTrack;
        private DelegateOnTrack onTrack;
        private DelegateOnNegotiationNeeded onNegotiationNeeded;
        private DelegateOnNegotiationNeeded selfOnNegotiationNeeded;
        private DelegateCreateSDSuccess onCreateSDSuccess;
        private DelegateCreateSDFailure onCreateSDFailure;
        private DelegateSetSDSuccess onSetSDSuccess;
        private DelegateSetSDFailure onSetSDFailure;

        private RTCIceCandidateRequestAsyncOperation opIceCandidateRequest;
        private RTCSessionDescriptionAsyncOperation m_opSessionDesc;
        private RTCSessionDescriptionAsyncOperation m_opSetDesc;
        private RTCSessionDescriptionAsyncOperation m_opSetRemoteDesc;

        private bool disposed;

        ~RTCPeerConnection()
        {
            this.Dispose();
        }

        public void Dispose()
        {
            if (this.disposed)
            {
                return;
            }
            if(self != IntPtr.Zero && !WebRTC.Context.IsNull)
            {
                Close();
                NativeMethods.ContextDeletePeerConnection(WebRTC.Context.self, m_id);
            }
            this.disposed = true;
        }

        public RTCIceConnectionState IceConnectionState
        {
            get
            { 
                return NativeMethods.PeerConnectionIceConditionState(self);
            }
        }

        public RTCPeerConnectionState ConnectionState
        {
            get
            {
                return NativeMethods.PeerConnectionState(self);
            }
        }

        public DelegateOnIceConnectionChange OnIceConnectionChange
        {
            get => onIceConnectionChange;
            set
            {
                onIceConnectionChange = value;
                selfOnIceConnectionChange = new DelegateOnIceConnectionChange(PCOnIceConnectionChange);
                NativeMethods.PeerConnectionRegisterIceConnectionChange(self, selfOnIceConnectionChange);
            }
        }
        public DelegateOnIceCandidate OnIceCandidate
        {
            get => onIceCandidate;
            set
            {
                onIceCandidate = value;
                selfOnIceCandidate = new DelegateNativeOnIceCandidate(PCOnIceCandidate);
                NativeMethods.PeerConnectionRegisterOnIceCandidate(self, selfOnIceCandidate);
            }
        }
        public DelegateOnDataChannel OnDataChannel
        {
            get => onDataChannel;
            set
            {
                onDataChannel = value;
                selfOnDataChannel = new DelegateNativeOnDataChannel(PCOnDataChannel);
                NativeMethods.PeerConnectionRegisterOnDataChannel(self, selfOnDataChannel);
            }
        }

        public DelegateOnNegotiationNeeded OnNegotiationNeeded
        {
            get => onNegotiationNeeded;
            set
            {
                onNegotiationNeeded = value;
                selfOnNegotiationNeeded = new DelegateOnNegotiationNeeded(PCOnNegotiationNeeded);
                NativeMethods.PeerConnectionRegisterOnRenegotiationNeeded(self, selfOnNegotiationNeeded);
            }
        }

        public DelegateOnTrack OnTrack
        {
            get => onTrack;
            set
            {
                onTrack = value;
                selfOnTrack = new DelegateNativeOnTrack(PCOnTrack);
                NativeMethods.PeerConnectionRegisterOnTrack(self, selfOnTrack);
            }
        }

        void PCOnIceCandidate(string sdp, string sdpMid, int sdpMlineIndex)
        {
            WebRTC.SyncContext.Post(_ =>
            {
                RTCIceCandidate candidate = new RTCIceCandidate { candidate = sdp, sdpMid = sdpMid, sdpMLineIndex = sdpMlineIndex };
                OnIceCandidate(candidate);
            }, null);
        }
        void PCOnIceConnectionChange(RTCIceConnectionState state)
        {
            WebRTC.SyncContext.Post(_ =>
            {
                OnIceConnectionChange(state);
            }, null);
        }
        void PCOnNegotiationNeeded()
        {
            WebRTC.SyncContext.Post(_ =>
            {
                OnNegotiationNeeded();
            }, null);
        }
        void PCOnDataChannel(IntPtr ptr)
        {
            WebRTC.SyncContext.Post(_ =>
            {
                OnDataChannel(new RTCDataChannel(ptr));
            }, null);
        }

        void PCOnTrack(IntPtr ptr)
        {
            WebRTC.SyncContext.Post(_ =>
            {
                OnTrack(new RTCTrackEvent(ptr));
            }, null);
        }

        public RTCConfiguration GetConfiguration()
        {
            int len = 0;
            IntPtr ptr = IntPtr.Zero;
            NativeMethods.PeerConnectionGetConfiguration(self, ref ptr, ref len);
            var str = Marshal.PtrToStringAnsi(ptr, len);
            ptr = IntPtr.Zero;
            return JsonUtility.FromJson<RTCConfiguration>(str);
        }

        public RTCErrorType SetConfiguration(ref RTCConfiguration config)
        {
            return NativeMethods.PeerConnectionSetConfiguration(self, JsonUtility.ToJson(config));
        }

        public RTCPeerConnection()
        {
            m_id = GetHashCode();
            self = NativeMethods.ContextCreatePeerConnection(WebRTC.Context.self, m_id);
            if (self == IntPtr.Zero)
            {
                throw new ArgumentException("Could not instantiate RTCPeerConnection");
            }
            InitCallback();
        }

        public RTCPeerConnection(ref RTCConfiguration config)
        {
            m_id = GetHashCode();
            string configStr = JsonUtility.ToJson(config);
            self = NativeMethods.ContextCreatePeerConnectionWithConfig(WebRTC.Context.self, m_id, configStr);
            if(self == IntPtr.Zero)
            {
                throw new ArgumentException("Could not instantiate RTCPeerConnection");
            }
            InitCallback();
        }

        void InitCallback()
        {
            onCreateSDSuccess = new DelegateCreateSDSuccess(OnSuccessCreateSessionDesc);
            onCreateSDFailure = new DelegateCreateSDFailure(OnFailureCreateSessionDesc);
            onSetSDSuccess = new DelegateSetSDSuccess(OnSuccessSetSessionDesc);
            onSetSDFailure = new DelegateSetSDFailure(OnFailureSetSessionDesc);
            NativeMethods.PeerConnectionRegisterCallbackCreateSD(self, onCreateSDSuccess, onCreateSDFailure);
            NativeMethods.PeerConnectionRegisterCallbackSetSD(self, onSetSDSuccess, onSetSDFailure);
        }

        public void Close()
        {
            if(self != IntPtr.Zero)
            {
                NativeMethods.PeerConnectionClose(self);
            }
        }

        public RTCRtpSender AddTrack(MediaStreamTrack track)
        {
            return new RTCRtpSender(NativeMethods.PeerConnectionAddTrack(self, track.self));
        }
        public void RemoveTrack(RTCRtpSender sender)
        {
            NativeMethods.PeerConnectionRemoveTrack(self, sender.self);
        }

        public void AddIceCandidate(ref RTCIceCandidate​ candidate)
        {
            NativeMethods.PeerConnectionAddIceCandidate(self, ref candidate);
        }

        public RTCSessionDescriptionAsyncOperation CreateOffer(ref RTCOfferOptions options)
        {
            m_opSessionDesc = new RTCSessionDescriptionAsyncOperation();
            NativeMethods.PeerConnectionCreateOffer(self, ref options);
            return m_opSessionDesc;
        }

        public RTCSessionDescriptionAsyncOperation CreateAnswer(ref RTCAnswerOptions options)
        {
            m_opSessionDesc = new RTCSessionDescriptionAsyncOperation();
            NativeMethods.PeerConnectionCreateAnswer(self, ref options);
            return m_opSessionDesc;
        }

        public RTCDataChannel CreateDataChannel(string label, ref RTCDataChannelInit options)
        {
            return new RTCDataChannel(NativeMethods.PeerConnectionCreateDataChannel(self, label, ref options));
        }

        void OnSuccessCreateSessionDesc(RTCSdpType type, string sdp)
        {
            WebRTC.SyncContext.Post(_ =>
            {
                m_opSessionDesc.desc.sdp = sdp;
                m_opSessionDesc.desc.type = type;
                m_opSessionDesc.Done();
            }, null);
        }

        void OnFailureCreateSessionDesc()
        {
            WebRTC.SyncContext.Post(_ =>
            {
                m_opSessionDesc.isError = true;
                m_opSessionDesc.Done();
            }, null);
        }

        public RTCSessionDescriptionAsyncOperation SetLocalDescription(ref RTCSessionDescription desc)
        {
            m_opSetDesc = new RTCSessionDescriptionAsyncOperation();
            NativeMethods.PeerConnectionSetLocalDescription(self, ref desc);
            return m_opSetDesc;
        }

        public RTCSessionDescription GetLocalDescription()
        {
            RTCSessionDescription desc = default;
            NativeMethods.PeerConnectionGetLocalDescription(self, ref desc);
            return desc;
        }

        public RTCSessionDescriptionAsyncOperation SetRemoteDescription(ref RTCSessionDescription desc)
        {
            m_opSetDesc = new RTCSessionDescriptionAsyncOperation();
            NativeMethods.PeerConnectionSetRemoteDescription(self, ref desc);
            return m_opSetDesc;
        }

        void OnSuccessSetSessionDesc()
        {
            WebRTC.SyncContext.Post(_ =>
            {
                m_opSetDesc.Done();
            }, null);
        }
        void OnFailureSetSessionDesc()
        {
            WebRTC.SyncContext.Post(_ =>
            {
                m_opSetDesc.isError = true;
                m_opSetDesc.Done();
            }, null);
        }
    }

}
