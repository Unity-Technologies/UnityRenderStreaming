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

        private RTCIceCandidateRequestAsyncOperation opIceCandidateRequest;
        private RTCSessionDescriptionAsyncOperation m_opSessionDesc;
        private RTCSessionDescriptionAsyncOperation m_opSetDesc;
        private RTCSessionDescriptionAsyncOperation m_opSetRemoteDesc;

        public void Dispose()
        {
            NativeMethods.peerConnectionClose(self);
        }

        public RTCIceConnectionState IceConnectionState
        {
            get
            {
                return NativeMethods.peerConnectionIceConditionState(self);
            }
        }

        public RTCPeerConnectionState ConnectionState
        {
            get
            {
                return NativeMethods.peerConnectionState(self);
            }
        }

        public RTCConfiguration GetConfiguration()
        {
            int len = 0;
            IntPtr ptr = IntPtr.Zero;
            NativeMethods.peerConnectionGetConfiguration(self, ref ptr, ref len);
            var str = Marshal.PtrToStringAnsi(ptr, len);
            Marshal.FreeHGlobal(ptr);
            ptr = IntPtr.Zero;
            return JsonUtility.FromJson<RTCConfiguration>(str);
        }

        public void SetConfiguration(ref RTCConfiguration config)
        {
            NativeMethods.peerConnectionSetConfiguration(self, JsonUtility.ToJson(config));
        }

        public RTCPeerConnection()
        {
            m_id = GetHashCode();
            m_context = WebRTC.Context;
            self = NativeMethods.contextCreatePeerConnection(m_context.self, m_id);
            InitCallback();
        }

        public RTCPeerConnection(ref RTCConfiguration config)
        {
            m_id = GetHashCode();
            m_context = WebRTC.Context;
            self = NativeMethods.contextCreatePeerConnection(m_context.self, m_id);
            InitCallback();
        }

        void InitCallback()
        {
            NativeMethods.peerConnectionRegisterCallbackCreateSD(self, OnSuccessCreateSessionDesc, OnFailureCreateSessionDesc);
            NativeMethods.peerConnectionRegisterCallbackSetSD(self, OnSuccessSetSessionDesc, OnFailureSetSessionDesc);
            NativeMethods.peerConnectionRegisterDataChannelMsgReceived(self, OnDataChannelMsg);
        }

        public void RegisterOnIceConnectionChange(DelegateOnIceConnectionChange callback)
        {
            NativeMethods.peerConnectionRegisterIceConnectionChange(self, callback);
        }

        public void RegisterOnIceCandidateReady(DelegateIceCandidateReady callback)
        {
            NativeMethods.peerConnectionRegisterOnIceCandidateReady(self, callback);
        }

        public void Close()
        {
            NativeMethods.peerConnectionClose(self);
        }

        public void AddIceCandidate(ref RTCIceCandidate​ candidate)
        {
            NativeMethods.peerConnectionAddIceCandidate(self, ref candidate);
        }

        public void AddTrack(MediaStreamTrack track, MediaStream stream)
        {
            NativeMethods.peerConnectionAddTrack(self, track, stream);
        }

        public void RemoveTrack()
        {

        }

        public RTCSessionDescriptionAsyncOperation CreateOffer(ref RTCOfferOptions options)
        {
            m_opSessionDesc = new RTCSessionDescriptionAsyncOperation();
            NativeMethods.peerConnectionCreateOffer(self, ref options);
            return m_opSessionDesc;
        }

        public RTCSessionDescriptionAsyncOperation CreateAnswer(ref RTCAnswerOptions options)
        {
            m_opSessionDesc = new RTCSessionDescriptionAsyncOperation();
            NativeMethods.peerConnectionCreateAnswer(self, ref options);
            return m_opSessionDesc;
        }

        public void CreateDataChannel(string label, ref RTCDataChannelInit options)
        {
            NativeMethods.peerConnectionCreateDataChannel(self, label, ref options);
        }

        public void SendData(string data)
        {
            NativeMethods.peerConnectionsendDataFromDataChannel(self, data);
        }

        void OnDataChannelMsg(string msg)
        {
            WebRTC.S_dataChannelMsgs.Enqueue(msg);
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
            NativeMethods.peerConnectionSetLocalDescription(self, ref desc);
            return m_opSetDesc;
        }

        public RTCSessionDescription GetLocalDescription()
        {
            RTCSessionDescription desc = default;
            NativeMethods.peerConnectionGetLocalDescription(self, ref desc);
            return desc;
        }

        public RTCSessionDescriptionAsyncOperation SetRemoteDescription(ref RTCSessionDescription desc)
        {
            m_opSetDesc = new RTCSessionDescriptionAsyncOperation();
            NativeMethods.peerConnectionSetRemoteDescription(self, ref desc);
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
