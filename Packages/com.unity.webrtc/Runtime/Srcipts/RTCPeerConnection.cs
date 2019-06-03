using UnityEngine;
using System.Runtime.InteropServices;
using System;

namespace Unity.WebRTC
{
    public class RTCPeerConnection : IDisposable
    {
        private int m_id;
        private IntPtr self;
        private Context m_context;
        private DelegateOnIceConnectionChange onIceConnectionChange;
        private DelegateOnIceConnectionChange selfOnIceConnectionChange;
        private DelegateOnIceCandidate onIceCandidate;
        private DelegateNativeOnIceCandidate selfOnIceCandidate;
        private DelegateOnDataChannel onDataChannel;
        private DelegateNativeOnDataChannel selfOnDataChannel;

        private RTCIceCandidateRequestAsyncOperation opIceCandidateRequest;
        private RTCSessionDescriptionAsyncOperation m_opSessionDesc;
        private RTCSessionDescriptionAsyncOperation m_opSetDesc;
        private RTCSessionDescriptionAsyncOperation m_opSetRemoteDesc;

        public void Dispose()
        {
            NativeMethods.PeerConnectionClose(self);
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

        void PCOnIceCandidate(string sdp, string sdpMid, int sdpMlineIndex)
        {
            WebRTC.SyncContext.Post(_ =>
            {
                RTCIceCandidate candidate = new RTCIceCandidate {candidate = sdp, sdpMid = sdpMid, sdpMlineIndex = sdpMlineIndex };
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
        void PCOnDataChannel(IntPtr ptr)
        {
            WebRTC.SyncContext.Post(_ =>
            {
                OnDataChannel(new RTCDataChannel(ptr));
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

        public void SetConfiguration(ref RTCConfiguration config)
        {
            NativeMethods.PeerConnectionSetConfiguration(self, JsonUtility.ToJson(config));
        }

        public RTCPeerConnection()
        {
            m_id = GetHashCode();
            m_context = WebRTC.Context;
            self = NativeMethods.ContextCreatePeerConnection(m_context.self, m_id);
            InitCallback();
        }

        public RTCPeerConnection(ref RTCConfiguration config) 
        {
            m_id = GetHashCode();
            m_context = WebRTC.Context;
            string configStr = JsonUtility.ToJson(config);
            self = NativeMethods.ContextCreatePeerConnectionWithConfig(m_context.self,m_id, configStr);
            InitCallback();
        }

        void InitCallback()
        {
            NativeMethods.PeerConnectionRegisterCallbackCreateSD(self, OnSuccessCreateSessionDesc, OnFailureCreateSessionDesc);
            NativeMethods.PeerConnectionRegisterCallbackSetSD(self, OnSuccessSetSessionDesc, OnFailureSetSessionDesc);
        }

        public void Close()
        {
            NativeMethods.PeerConnectionClose(self);
        }

        public void AddIceCandidate(ref RTCIceCandidate​ candidate)
        {
            NativeMethods.PeerConnectionAddIceCandidate(self, ref candidate);
        }

        public void AddTrack(MediaStreamTrack track, MediaStream stream)
        {
            NativeMethods.PeerConnectionAddTrack(self, track, stream);
        }

        public void RemoveTrack()
        {

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
