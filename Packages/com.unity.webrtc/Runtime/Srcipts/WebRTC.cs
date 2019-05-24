using System;
using System.Runtime.InteropServices;
using UnityEngine;
using System.Collections.Concurrent;
using System.Threading;
using UnityEngine.Events;
using System.Collections.Generic;

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
        [MarshalAs(UnmanagedType.LPStr)]
        public string candidate;
        [MarshalAs(UnmanagedType.LPStr)]
        public string sdpMid;
        public int sdpMlineIndex;
    }

    public struct RTCData​Channel
    {
        public Action onMessage;
        public Action onOpen;
        public Action onclose;
        public Action onBufferedAmountLow;
        public Action onError;
    }

    public struct RTCDataChannelInit
    {
        public bool reliable;
        public bool ordered;
        public int maxRetransmitTime;
        public int maxRetransmits;
        [MarshalAs(UnmanagedType.LPStr)]
        public string protocol;
        public bool negotiated;
        public int id;

        public RTCDataChannelInit(bool reliable)
        {
            this.reliable = reliable;
            ordered = true;
            maxRetransmitTime = -1;
            maxRetransmits = -1;
            negotiated = false;
            id = -1;
            protocol = "";
        }
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

    public class WebRTCSyncContext :  SynchronizationContext
    {
        public static ConcurrentQueue<Action> tasks;
        private static WebRTCSyncContext syncContext;
        static WebRTCSyncContext()
        {
            tasks = new ConcurrentQueue<Action>();
            syncContext = new WebRTCSyncContext();
            SynchronizationContext.SetSynchronizationContext(syncContext);
        }
        public override void Send(SendOrPostCallback d, object state)
        {
            tasks.Enqueue(() => 
            {
                d(state);
            });
        }
        public override void Post(SendOrPostCallback d, object state)
        {
            tasks.Enqueue(() =>
            {
                d(state);
            });
        }
        public void Update()
        {
            if(!tasks.IsEmpty)
            {
                Action curTask;
                if(tasks.TryDequeue(out curTask))
                    curTask();
            }
        }
        public void flush()
        {
            while(!tasks.IsEmpty)
            {
                Action curTask;
                if (tasks.TryDequeue(out curTask))
                    curTask();
            }
        }

    }

    public class RTCPeerConnection : IDisposable
    {
        private int m_id;
        private IntPtr self;
        private Context m_context;

        private RTCIceCandidateRequestAsyncOperation opIceCandidateRequest;
        private RTCSessionDescriptionAsyncOperation m_opSessionDesc;
        private RTCSessionDescriptionAsyncOperation m_opSetDesc;
        private RTCSessionDescriptionAsyncOperation m_opSetRemoteDesc;

        public RTCTrackEvent onTrack;
        public ConnectionStateChangeEvent onConnectionStateChange;
        public RTCDataChannelEvent onDataChannel;
        public RTCPeer​Connection​IceEvent onIceCandidate;
        public RTCIceConnectionStateChangeEvent onIceConnectionStateChange;

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
            NativeMethods.peerConnectionRegisterCallbackCreateSD(self,OnSuccessCreateSessionDesc, OnFailureCreateSessionDesc);
            NativeMethods.peerConnectionRegisterCallbackSetSD(self,OnSuccessSetSessionDesc, OnFailureSetSessionDesc);
            NativeMethods.peerConnectionRegisterDataChannelMsgReceived(self,OnDataChannelMsg);
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

    public enum RTCSdpType
    {
        Offer,
        Pranswer,
        Answer
    }

    public struct RTCSessionDescription
    {
        public RTCSdpType type;
        [MarshalAs(UnmanagedType.LPStr)]
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

    [Serializable]
    public struct RTCIceServer
    {
        public string credential;
        public RTCIceCredentialType credentialType;
        public string[] urls;
        public string username;
    }

    public enum RTCIceTransportPolicy
    {
        Relay,
        All
    }

    [Serializable]
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
        private static WebRTCSyncContext s_syncContext;
        private static ConcurrentQueue<string> s_dataChannelMsgs;
        private static readonly object s_syncMsgObj = new object();
        private static readonly object s_syncObj = new object();

        public static void Initialize()
        {
            NativeMethods.registerDebugLog(DebugLog);
            s_context = Context.Create();
            s_syncContext = new WebRTCSyncContext();
            s_dataChannelMsgs = new ConcurrentQueue<string>();
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
        internal static WebRTCSyncContext SyncContext { get { return s_syncContext; } }

        internal static ConcurrentQueue<string> S_dataChannelMsgs { get => s_dataChannelMsgs; }

        internal static object S_syncMsgObj => s_syncMsgObj;

        internal static object S_syncObj => s_syncObj;
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
    public delegate void DelegateOnDataChannelMsg([MarshalAs(UnmanagedType.LPStr)] string msg);
    [UnmanagedFunctionPointer(CallingConvention.StdCall)]
    public delegate void DelegateOnIceConnectionChange(RTCIceConnectionState state);
    [UnmanagedFunctionPointer(CallingConvention.StdCall)] 
    public delegate void DelegateIceCandidateReady([MarshalAs(UnmanagedType.LPStr)] string candidate, [MarshalAs(UnmanagedType.LPStr)] string sdpMid, int sdpMlineIndex);
    [UnmanagedFunctionPointer(CallingConvention.StdCall)]
    internal delegate void DelegatePeerConnectionCallbackEvent(RTCPeerConnectionEventType type, [MarshalAs(UnmanagedType.LPStr, SizeConst = 1024)] string str);

    internal static class NativeMethods
    {
        [DllImport(WebRTC.Lib)]
        public static extern void registerDebugLog(DelegateDebugLog func);
        [DllImport(WebRTC.Lib)]
        public static extern Context contextCreate(int uid);
        [DllImport(WebRTC.Lib)]
        public static extern void contextDestroy(int uid);
        [DllImport(WebRTC.Lib)]
        public static extern IntPtr contextCreatePeerConnection(IntPtr ptr, int id);
        [DllImport(WebRTC.Lib)]
        public static extern IntPtr contextCreatePeerConnectionWithConfig(IntPtr ptr, int id, string conf);
        [DllImport(WebRTC.Lib)]
        public static extern void peerConnectionClose(IntPtr ptr);
        [DllImport(WebRTC.Lib)]
        public static extern void peerConnectionSetConfiguration(IntPtr ptr, [MarshalAs(UnmanagedType.LPStr, SizeConst = 256)] string conf);
        [DllImport(WebRTC.Lib)] 
        public static extern void peerConnectionCreateDataChannel(IntPtr ptr, [MarshalAs(UnmanagedType.LPStr, SizeConst = 256)] string label, ref RTCDataChannelInit options);
        [DllImport(WebRTC.Lib)]
        public static extern void peerConnectionGetConfiguration(IntPtr ptr, ref IntPtr conf, ref int len);
        [DllImport(WebRTC.Lib)]
        public static extern void peerConnectionCreateOffer(IntPtr ptr, ref RTCOfferOptions options);
        [DllImport(WebRTC.Lib)]
        public static extern void peerConnectionCreateAnswer(IntPtr ptr, ref RTCAnswerOptions options);
        [DllImport(WebRTC.Lib)]
        public static extern void peerConnectionsendDataFromDataChannel(IntPtr ptr, [MarshalAs(UnmanagedType.LPStr)]string data);
        [DllImport(WebRTC.Lib)]
        public static extern void peerConnectionRegisterCallbackCreateSD(IntPtr ptr, DelegateCreateSDSuccess onSuccess, DelegateCreateSDFailure onFailure);
        [DllImport(WebRTC.Lib)]
        public static extern void peerConnectionRegisterCallbackSetSD(IntPtr ptr, DelegateSetSDSuccess onSuccess, DelegateSetSDFailure onFailure);
        [DllImport(WebRTC.Lib)]
        public static extern void peerConnectionRegisterDataChannelMsgReceived(IntPtr ptr, DelegateOnDataChannelMsg callback);
        [DllImport(WebRTC.Lib)]
        public static extern void peerConnectionRegisterIceConnectionChange(IntPtr ptr, DelegateOnIceConnectionChange callback);
        [DllImport(WebRTC.Lib)]
        public static extern void peerConnectionRegisterOnIceCandidateReady(IntPtr ptr, DelegateIceCandidateReady callback);
        [DllImport(WebRTC.Lib)]
        public static extern void peerConnectionSetLocalDescription(IntPtr ptr, ref RTCSessionDescription desc);
        [DllImport(WebRTC.Lib)]
        public static extern void peerConnectionGetLocalDescription(IntPtr ptr, ref RTCSessionDescription desc);
        [DllImport(WebRTC.Lib)]
        public static extern void peerConnectionSetRemoteDescription(IntPtr ptr, ref RTCSessionDescription desc);
        [DllImport(WebRTC.Lib)]
        public static extern bool peerConnectionAddTrack(IntPtr ptr, MediaStreamTrack track, MediaStream stream);
        [DllImport(WebRTC.Lib)]
        public static extern bool peerConnectionAddIceCandidate(IntPtr ptr, ref RTCIceCandidate​ candidate);
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
 
    }
}


