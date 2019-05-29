using System;
using System.Runtime.InteropServices;
using UnityEngine;
using System.Collections.Concurrent;
using System.Threading;

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

    public struct RTCIceCandidate​
    {
        [MarshalAs(UnmanagedType.LPStr)]
        public string candidate;
        [MarshalAs(UnmanagedType.LPStr)]
        public string sdpMid;
        public int sdpMlineIndex;
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
        private static SynchronizationContext s_syncContext;
        private static ConcurrentQueue<string> s_dataChannelMsgs;
        private static readonly object s_syncMsgObj = new object();
        private static readonly object s_syncObj = new object();

        public static void Initialize()
        {
            NativeMethods.registerDebugLog(DebugLog);
            s_context = Context.Create();
            s_syncContext = SynchronizationContext.Current;
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
        public static SynchronizationContext SyncContext { get { return s_syncContext; } }

        public static ConcurrentQueue<string> S_dataChannelMsgs { get => s_dataChannelMsgs; }

        internal static object S_syncMsgObj => s_syncMsgObj;

        internal static object S_syncObj => s_syncObj;
    }

    [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
    internal delegate void DelegateDebugLog([MarshalAs(UnmanagedType.LPStr)] string str);
    [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
    internal delegate void DelegateCreateSDSuccess(RTCSdpType type, [MarshalAs(UnmanagedType.LPStr)] string sdp);
    [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
    internal delegate void DelegateCreateSDFailure();
    [UnmanagedFunctionPointer(CallingConvention.Cdecl)] 
    internal delegate void DelegateSetSDSuccess();
    [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
    internal delegate void DelegateSetSDFailure();
    [UnmanagedFunctionPointer(CallingConvention.Cdecl)] 
    public delegate void DelegateOnIceConnectionChange(RTCIceConnectionState state);
    [UnmanagedFunctionPointer(CallingConvention.Cdecl)] 
    public delegate void DelegateIceCandidateReady([MarshalAs(UnmanagedType.LPStr)] string candidate, [MarshalAs(UnmanagedType.LPStr)] string sdpMid, int sdpMlineIndex);
    [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
    internal delegate void DelegatePeerConnectionCallbackEvent(RTCPeerConnectionEventType type, [MarshalAs(UnmanagedType.LPStr, SizeConst = 1024)] string str);
    [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
    public delegate void DelegateOnDataChannel(IntPtr ptr);
    [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
    public delegate void DelegateOnMessage([MarshalAs(UnmanagedType.LPStr)] string msg);
    [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
    public delegate void DelegateOnOpen();
    [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
    public delegate void DelegateOnClose();

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
        public static extern IntPtr peerConnectionSetConfiguration(IntPtr ptr, [MarshalAs(UnmanagedType.LPStr, SizeConst = 256)] string conf);
        [DllImport(WebRTC.Lib)] 
        public static extern IntPtr peerConnectionCreateDataChannel(IntPtr ptr, [MarshalAs(UnmanagedType.LPStr, SizeConst = 256)] string label, ref RTCDataChannelInit options);
        [DllImport(WebRTC.Lib)]
        public static extern void peerConnectionGetConfiguration(IntPtr ptr, ref IntPtr conf, ref int len);
        [DllImport(WebRTC.Lib)]
        public static extern void peerConnectionCreateOffer(IntPtr ptr, ref RTCOfferOptions options);
        [DllImport(WebRTC.Lib)]
        public static extern void peerConnectionCreateAnswer(IntPtr ptr, ref RTCAnswerOptions options);
        [DllImport(WebRTC.Lib)]
        public static extern void peerConnectionRegisterCallbackCreateSD(IntPtr ptr, DelegateCreateSDSuccess onSuccess, DelegateCreateSDFailure onFailure);
        [DllImport(WebRTC.Lib)]
        public static extern void peerConnectionRegisterCallbackSetSD(IntPtr ptr, DelegateSetSDSuccess onSuccess, DelegateSetSDFailure onFailure);
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
        [DllImport(WebRTC.Lib)]
        public static extern void peerConnectionRegisterOnDataChannel(IntPtr ptr, DelegateOnDataChannel callback);
        [DllImport(WebRTC.Lib)]
        public static extern void DataChannelUnregisterObserver(IntPtr ptr);
        [DllImport(WebRTC.Lib)]
        public static extern void DataChannelRegisterObserver(IntPtr ptr);
        [DllImport(WebRTC.Lib)]
        public static extern int DataChannelGetID(IntPtr ptr);
        [DllImport(WebRTC.Lib)]
        public static extern IntPtr DataChannelGetLabel(IntPtr ptr);
        [DllImport(WebRTC.Lib)]
        public static extern void DataChannelSend(IntPtr ptr, [MarshalAs(UnmanagedType.LPStr)]string msg);
        [DllImport(WebRTC.Lib)]
        public static extern void DataChannelClose(IntPtr ptr);
        [DllImport(WebRTC.Lib)]
        public static extern void DataChannelRegisterOnMessage(IntPtr ptr, DelegateOnMessage callback);
        [DllImport(WebRTC.Lib)]
        public static extern void DataChannelRegisterOnOpen(IntPtr ptr, DelegateOnOpen callback);
        [DllImport(WebRTC.Lib)]
        public static extern void DataChannelRegisterOnClose(IntPtr ptr, DelegateOnClose callback);
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


