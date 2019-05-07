using System;
using System.Runtime.InteropServices;
using UnityEngine;
using UnityEngine.Events;

namespace Unity.WebRTC
{
    /// <summary>
    ///
    /// </summary>
    public class MediaStreamConstraints
    {
        /// <summary>
        ///
        /// </summary>
        public bool audio;

        /// <summary>
        ///
        /// </summary>
        public bool video;
    }

    public class MediaDevices
    {
        public static MediaDevicesAsyncOperation GetUserMedia(MediaStreamConstraints constraints)
        {
            return new MediaDevicesAsyncOperation();
        }
    }

    public struct MediaStreamEvent
    {
        public RTCIceCandidate​ candidate;
    }

    public delegate void RTCPeer​Connection​IceEvent(
        ref RTCIceCandidate​ candidate
    );

    public delegate void RTCDataChannelEvent(
        ref RTCData​Channel channel
    );

    public delegate void RTCIceConnectionStateChangeEvent();

    public delegate void RTCTrackEvent(
        ref RTCRtpReceiver receiver,
        MediaStream[] streams,
        ref MediaStreamTrack track,
        ref RTCRtpTransceiver transceiver
    );

    public delegate void ConnectionStateChangeEvent();

    public struct RTCIceCandidate​
    {
        public string candidate;
        public string sdpMid;
        public int sdpMLineIndex;
    }

    public struct RTCData​Channel
    {
        public Action onMessage;
        public Action onOpen;
        public Action onclose;
        public Action onBufferedAmountLow;
        public Action onError;
    }

    public struct RTCRtpReceiver
    {

    }

    public struct RTCRtpTransceiver
    {

    }

    public enum RTCErrorDetailType
    {
        DataChannelFailure,
        DtlsFailure,
        FingerprintFailure,
        IdpBadScriptFailure,
        IdpExecutionFailure,
        IdpLoadFailure,
        IdpNeedLogin,
        IdpTimeout,
        IdpTlsFailure,
        IdpTokenExpired,
        IdpTokenInvalid,
        SctpFailure,
        SdpSyntaxError,
        HardwareEncoderNotAvailable,
        HardwareEncoderError
    }

    public struct RTCError
    {
        public RTCErrorDetailType errorDetail;
        public long sdpLineNumber;
        public long httpRequestStatusCode;
        public long sctpCauseCode;
        public ulong receivedAlert;
        public ulong sentAlert;
    }

    public enum RTCPeerConnectionState
    {
        New,
        Connecting,
        Connected,
        Disconnected,
        Failed,
        Closed
    }

    public enum RTCIceConnectionState
    {
        New,
        Checking,
        Connected,
        Completed,
        Failed,
        Disconnected,
        Closed,
        Max
    }

    public enum RTCPeerConnectionEventType
    {
        ConnectionStateChange,
        DataChannel,
        IceCandidate,
        IceConnectionStateChange,
        Track
    }

    public class RTCPeerConnection : IDisposable
    {
        private int m_id;
        private Context m_context;
        private PeerConnectionObject m_obj;

        private RTCIceCandidateRequestAsyncOperation opIceCandidateRequest;
        private RTCSessionDescriptionAsyncOperation m_opSessionDesc;
        private RTCSessionDescriptionAsyncOperation m_opSetDesc;
        private RTCSessionDescriptionAsyncOperation m_opSetRemoteDesc;

        public RTCTrackEvent onTrack;
        public ConnectionStateChangeEvent onConnectionStateChange;
        public RTCDataChannelEvent onDataChannel;
        public RTCPeer​Connection​IceEvent onIceCandidate;
        public RTCIceConnectionStateChangeEvent onIceConnectionStateChange;
        //public onOffer;

        public void Dispose()
        {
            m_obj.Close();
        }

        public RTCIceConnectionState IceConnectionState
        {
            get
            {
                return m_obj.IceConditionState;
            }
        }

        public RTCPeerConnectionState ConnectionState
        {
            get
            {
                return m_obj.ConnectionState;
            }
        }

        public RTCConfiguration GetConfiguration()
        {
            RTCConfiguration config = default;
            m_obj.GetConfiguration(out config);
            return config;
        }

        public void SetConfiguration(ref RTCConfiguration config)
        {
            m_obj.SetConfiguration(ref config);
        }

        public RTCPeerConnection()
        {
            m_id = GetHashCode();
            m_context = WebRTC.Context;
            m_obj = m_context.CreatePeerConnection(m_id);
            InitCallback();
        }

        public RTCPeerConnection(ref RTCConfiguration config)
        {
            m_id = GetHashCode();
            m_context = WebRTC.Context;
            m_obj = m_context.CreatePeerConnection(m_id, ref config);
            InitCallback();
        }

        void InitCallback()
        {
            m_obj.RegisterCallbackCreateSD(OnSuccessCreateSessionDesc, OnFailureCreateSessionDesc);
            m_obj.RegisterCallbackSetSD(OnSuccessSetSessionDesc, OnFailureSetSessionDesc);
            m_obj.RegisterCallbackEvent(CallbackEvent);
        }

        public void Close()
        {
            m_obj.Close();
        }

        public RTCIceCandidateRequestAsyncOperation AddIceCandidate(ref RTCIceCandidate​ candidate)
        {
            var callback = new DelegatePeerConnectionAddIceCandidate(CallbackAddIceCandidate);
            m_obj.AddIceCandidate(ref candidate, callback);
            opIceCandidateRequest = new RTCIceCandidateRequestAsyncOperation();
            return opIceCandidateRequest;
        }

        void CallbackAddIceCandidate()
        {
            opIceCandidateRequest.Done();
        }

        void CallbackEvent(RTCPeerConnectionEventType type, string json)
        {
            Debug.Log(json);
        }

        public void AddTrack(MediaStreamTrack track, MediaStream stream)
        {
            m_obj.AddTrack(track, stream);
        }

        public void RemoveTrack()
        {

        }

        public RTCSessionDescriptionAsyncOperation CreateOffer(ref RTCOfferOptions options)
        {
            m_opSessionDesc = new RTCSessionDescriptionAsyncOperation();
            m_obj.CreateOffer(ref options);
            return m_opSessionDesc;
        }

        public RTCSessionDescriptionAsyncOperation CreateAnswer(ref RTCAnswerOptions options)
        {
            m_opSessionDesc = new RTCSessionDescriptionAsyncOperation();
            m_obj.CreateAnswer(ref options);
            return m_opSessionDesc;
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
            m_obj.SetLocalDescription(ref desc);
            return m_opSetDesc;
        }

        public RTCSessionDescription GetLocalDescription()
        {
            return m_obj.GetLocalDescription();
        }

        public RTCSessionDescriptionAsyncOperation SetRemoteDescription(ref RTCSessionDescription desc)
        {
            m_opSetDesc = new RTCSessionDescriptionAsyncOperation();
            m_obj.SetRemoteDescription(ref desc);
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

    public enum RTCSdpType
    {
        Offer,
        Pranswer,
        Answer
    }

    public struct RTCSessionDescription
    {
        public RTCSdpType type;
        [MarshalAs(UnmanagedType.LPStr, SizeConst = 2048)]
        public string sdp;


    }

    public struct RTCOfferOptions
    {
        public bool iceRestart;
        public bool offerToReceiveAudio;
        public bool offerToReceiveVideo;
    }

    public struct RTCAnswerOptions
    {
        public bool iceRestart;
    }

public enum RTCIceCredentialType
    {
        Password,
        OAuth
    }

    public struct RTCIceServer
    {
        public string credential;
        public string credentialType;
        public string[] urls;
        public int urlsLength;
        public string username;
    }

    public enum RTCIceTransportPolicy
    {
        Relay,
        All
    }

    public struct RTCConfiguration
    {
        public RTCIceServer[] iceServers;
        public RTCIceTransportPolicy iceTransportPolicy;
    }

    public class MediaStream
    {
        public MediaStreamTrack[] GetVideoTracks()
        {
            return new MediaStreamTrack[0];
        }
        public MediaStreamTrack[] GetAudioTracks()
        {
            return new MediaStreamTrack[0];
        }

        public MediaStreamTrack[] GetTracks()
        {
            return new MediaStreamTrack[0];
        }
    }

    public class MediaStreamTrack
    {

    }

    public static class WebRTC
    {
#if UNITY_EDITOR_OSX
        internal const string Lib = "Packages/com.unity.webrtc/Runtime/Plugins/x86_64/abci.bundle/Contents/MacOS/webrtc";
#elif UNITY_EDITOR_LINUX
        internal const string Lib = "Packages/com.unity.webrtc/Runtime/Plugins/x86_64/webrtc.so";
#elif UNITY_EDITOR_WIN
        internal const string Lib = "Packages/com.unity.webrtc/Runtime/Plugins/x86_64/webrtc.dll";
#elif UNITY_STANDALONE
        internal const string Lib = "webrtc";
#endif

        private static Context s_context;
        private static System.Threading.SynchronizationContext s_syncContext;

        public static void Initialize()
        {
            NativeMethods.registerDebugLog(DebugLog);
            s_context = Context.Create();
            s_syncContext = System.Threading.SynchronizationContext.Current;
        }

        public static void Finalize(int id = 0)
        {
            s_context.Destroy();
            NativeMethods.registerDebugLog(null);
        }

        static void DebugLog(string str)
        {
            Debug.Log(str);
        }

        internal static Context Context { get { return s_context; }  }
        internal static System.Threading.SynchronizationContext SyncContext { get { return s_syncContext; } }
    }

    [UnmanagedFunctionPointer(CallingConvention.StdCall)]
    internal delegate void DelegateDebugLog([MarshalAs(UnmanagedType.LPStr)] string str);
    [UnmanagedFunctionPointer(CallingConvention.StdCall)]
    internal delegate void DelegateCreateSDSuccess(RTCSdpType type, [MarshalAs(UnmanagedType.LPStr)] string sdp);
    [UnmanagedFunctionPointer(CallingConvention.StdCall)]
    internal delegate void DelegateCreateSDFailure();
    [UnmanagedFunctionPointer(CallingConvention.StdCall)]
    internal delegate void DelegateSetSDSuccess();
    [UnmanagedFunctionPointer(CallingConvention.StdCall)]
    internal delegate void DelegateSetSDFailure();
    [UnmanagedFunctionPointer(CallingConvention.StdCall)]
    internal delegate void DelegatePeerConnectionAddIceCandidate();
    [UnmanagedFunctionPointer(CallingConvention.StdCall)]
    internal delegate void DelegatePeerConnectionCallbackEvent(RTCPeerConnectionEventType type, [MarshalAs(UnmanagedType.LPStr, SizeConst = 1024)] string str);

    internal static class NativeMethods
    {
        [DllImport(WebRTC.Lib)]
        public static extern Context registerDebugLog(DelegateDebugLog func);
        [DllImport(WebRTC.Lib)]
        public static extern Context contextCreate(int uid);
        [DllImport(WebRTC.Lib)]
        public static extern void contextDestroy(int uid);
        [DllImport(WebRTC.Lib)]
        public static extern PeerConnectionObject contextCreatePeerConnection(IntPtr ptr, int id);
        [DllImport(WebRTC.Lib)]
        public static extern PeerConnectionObject contextCreatePeerConnectionWithConfig(IntPtr ptr, int id, string conf);
        [DllImport(WebRTC.Lib)]
        public static extern bool peerConnectionClose(IntPtr ptr);
        [DllImport(WebRTC.Lib)]
        public static extern bool peerConnectionSetConfiguration(IntPtr ptr, [MarshalAs(UnmanagedType.LPStr)] string conf);
        [DllImport(WebRTC.Lib)]
        public static extern void peerConnectionGetConfiguration(IntPtr ptr, [MarshalAs(UnmanagedType.LPStr)] out string conf);
        [DllImport(WebRTC.Lib)]
        public static extern void peerConnectionCreateOffer(IntPtr ptr, ref RTCOfferOptions options);
        [DllImport(WebRTC.Lib)]
        public static extern void peerConnectionCreateAnswer(IntPtr ptr, ref RTCAnswerOptions options);
        [DllImport(WebRTC.Lib)]
        public static extern void peerConnectionRegisterCallbackCreateSD(IntPtr ptr, DelegateCreateSDSuccess onSuccess, DelegateCreateSDFailure onFailure);
        [DllImport(WebRTC.Lib)]
        public static extern void peerConnectionRegisterCallbackSetSD(IntPtr ptr, DelegateSetSDSuccess onSuccess, DelegateSetSDFailure onFailure);
        [DllImport(WebRTC.Lib)]
        public static extern void peerConnectionSetLocalDescription(IntPtr ptr, ref RTCSessionDescription desc);
        [DllImport(WebRTC.Lib)]
        public static extern void peerConnectionGetLocalDescription(IntPtr ptr, ref RTCSessionDescription desc);
        [DllImport(WebRTC.Lib)]
        public static extern void peerConnectionSetRemoteDescription(IntPtr ptr, ref RTCSessionDescription desc);
        [DllImport(WebRTC.Lib)]
        public static extern bool peerConnectionAddTrack(IntPtr ptr, MediaStreamTrack track, MediaStream stream);
        [DllImport(WebRTC.Lib)]
        public static extern bool peerConnectionAddIceCandidate(IntPtr ptr, ref RTCIceCandidate​ candidate, DelegatePeerConnectionAddIceCandidate func);
        [DllImport(WebRTC.Lib)]
        public static extern void peerConnectionRegisterCallbackEvent(IntPtr ptr, DelegatePeerConnectionCallbackEvent func);
        [DllImport(WebRTC.Lib)]
        public static extern RTCPeerConnectionState peerConnectionState(IntPtr ptr);
        [DllImport(WebRTC.Lib)]
        public static extern RTCIceConnectionState peerConnectionIceConditionState(IntPtr ptr);
    }

    internal struct Context
    {
        internal IntPtr self;

        public bool IsNull { get { return self == IntPtr.Zero; }  }
        public static implicit operator bool(Context v) { return v.self != IntPtr.Zero; }
        public static bool ToBool(Context v) { return v; }
        public static Context Create(int uid=0) { return NativeMethods.contextCreate(uid); }
        public void Destroy(int uid = 0) { NativeMethods.contextDestroy(uid); self = IntPtr.Zero; }
        public PeerConnectionObject CreatePeerConnection(int id)
        {
            return NativeMethods.contextCreatePeerConnection(self, id);
        }
        public PeerConnectionObject CreatePeerConnection(int id, ref RTCConfiguration conf)
        {
            Debug.Log("contextCreatePeerConnectionWithConfig");
            return NativeMethods.contextCreatePeerConnectionWithConfig(self, id, JsonUtility.ToJson(conf));
        }
    }

    internal struct PeerConnectionObject
    {
        internal IntPtr self;
        public void SetConfiguration(ref RTCConfiguration conf)
        {
            NativeMethods.peerConnectionSetConfiguration(self, JsonUtility.ToJson(conf));
        }
        public void GetConfiguration(out RTCConfiguration conf)
        {
            string str = string.Empty;
            NativeMethods.peerConnectionGetConfiguration(self, out str);
            conf = JsonUtility.FromJson<RTCConfiguration>(str);
        }
        public void Close() { NativeMethods.peerConnectionClose(self); }
        public void AddTrack(MediaStreamTrack track, MediaStream stream) { NativeMethods.peerConnectionAddTrack(self, track, stream); }
        public void CreateOffer(ref RTCOfferOptions options)
        {
            NativeMethods.peerConnectionCreateOffer(self, ref options);
        }
        public void CreateAnswer(ref RTCAnswerOptions options)
        {
            NativeMethods.peerConnectionCreateAnswer(self, ref options);
        }

        public void RegisterCallbackCreateSD(DelegateCreateSDSuccess onSuccess, DelegateCreateSDFailure onFailure)
        {
            NativeMethods.peerConnectionRegisterCallbackCreateSD(self, onSuccess, onFailure);
        }
        public void SetLocalDescription(ref RTCSessionDescription desc)
        {
            NativeMethods.peerConnectionSetLocalDescription(self, ref desc);
        }
        public RTCSessionDescription GetLocalDescription()
        {
            RTCSessionDescription desc = default;
            NativeMethods.peerConnectionGetLocalDescription(self, ref desc);
            return desc;
        }

        public void RegisterCallbackSetSD(DelegateSetSDSuccess onSuccess, DelegateSetSDFailure onFailure)
        {
            NativeMethods.peerConnectionRegisterCallbackSetSD(self, onSuccess, onFailure);
        }
        public void SetRemoteDescription(ref RTCSessionDescription desc) { NativeMethods.peerConnectionSetRemoteDescription(self, ref desc); }
        //public void RegisterCallbackOnTrack(RTCTrackEvent func) { NativeMethods.peerConnectionRegisterCallbackOnTrack(self, func);  }
        public bool AddIceCandidate(ref RTCIceCandidate​ candidate, DelegatePeerConnectionAddIceCandidate func)
        {
            return NativeMethods.peerConnectionAddIceCandidate(self, ref candidate, func);
        }
        public void RegisterCallbackEvent(DelegatePeerConnectionCallbackEvent func)
        {
            NativeMethods.peerConnectionRegisterCallbackEvent(self, func);
        }
        public RTCIceConnectionState IceConditionState
        {
            get { return NativeMethods.peerConnectionIceConditionState(self); }
        }

        public RTCPeerConnectionState ConnectionState
        {
            get { return NativeMethods.peerConnectionState(self); }
        }
    }
}


