using System;
using System.Runtime.InteropServices;
using UnityEngine;
using System.Collections.Concurrent;
using System.Threading;
using System.Collections;

namespace Unity.WebRTC
{
    public struct RTCIceCandidate​
    {
        [MarshalAs(UnmanagedType.LPStr)]
        public string candidate;
        [MarshalAs(UnmanagedType.LPStr)]
        public string sdpMid;
        public int sdpMLineIndex;
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

    public enum RTCErrorType
    {
        None,
        UnsupportedOperation,
        UnsupportedParameter,
        InvalidParameter,
        InvalidRange,
        SyntaxError,
        InvalidState,
        InvalidModification,
        NetworkError,
        ResourceExhausted,
        InternalError
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
        [Tooltip("Optional: specifies the password to use when authenticating with the ICE server")]
        public string credential;
        [Tooltip("What type of credential the `password` value")]
        public RTCIceCredentialType credentialType;
        [Tooltip("Array to set URLs of your STUN/TURN servers")]
        public string[] urls;
        [Tooltip("Optional: specifies the username to use when authenticating with the ICE server")]
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

    public enum CodecInitializationResult
    {
        NotInitialized,
        Success,
        DriverNotInstalled,
        DriverVersionDoesNotSupportAPI,
        APINotFound,
        EncoderInitializationFailed
    }

    public class CodecInitializationException : Exception
    {
        public CodecInitializationResult result { get; private set; }

        internal CodecInitializationException(CodecInitializationResult result)
        {
            this.result = result;
        }
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
        private static IntPtr s_renderCallback;
        private static SynchronizationContext s_syncContext;
        private static Material flipMat;

        public static void Initialize()
        {
            NativeMethods.RegisterDebugLog(DebugLog);
            s_context = Context.Create();
            var result = Context.GetCodecInitializationResult();
            if (result != CodecInitializationResult.Success)
            {
                switch(result)
                {
                    case CodecInitializationResult.DriverNotInstalled:
                        Debug.LogError("[WebRTC] The hardware codec driver not installed");
                        break;
                    case CodecInitializationResult.DriverVersionDoesNotSupportAPI:
                        Debug.LogError("[WebRTC] The version of the hardware codec driver does not support API");
                        break;
                    case CodecInitializationResult.EncoderInitializationFailed:
                        Debug.LogError("[WebRTC] Hardware encoder initialization failed");
                        break;
                    case CodecInitializationResult.APINotFound:
                        Debug.LogError("[WebRTC] Hardware encoder API not found");
                        break;
                }
            }

            s_renderCallback = s_context.GetRenderEventFunc();
            NativeMethods.SetCurrentContext(s_context.self);
            s_syncContext = SynchronizationContext.Current;
            var flipShader = Resources.Load<Shader>("Flip");
            if(flipShader != null)
            {
                flipMat = new Material(flipShader); 
            }
        }
        public static IEnumerator Update() 
        {
            while (true)
            {
                // Wait until all frame rendering is done
                yield return new WaitForEndOfFrame();
                if (CameraExtension.started)
                {
                    //Blit is for DirectX Rendering API Only
                    foreach(var rts in CameraExtension.camCopyRts)
                    {
                        Graphics.Blit(rts[0], rts[1], flipMat);
                    }
                    GL.IssuePluginEvent(s_renderCallback, 0);
                }
                Audio.Update();
            }
        }

        public static void Finalize(int id = 0)
        {
            s_context.Destroy();
            NativeMethods.RegisterDebugLog(null);
        }

        static void DebugLog(string str)
        {
            Debug.Log(str);
        }

        internal static Context Context { get { return s_context; } }
        internal static SynchronizationContext SyncContext { get { return s_syncContext; } }

        public static bool HWEncoderSupport
        {
            get
            {
                if(s_context.IsNull)
                {
                    throw new CodecInitializationException(CodecInitializationResult.NotInitialized);
                }
                var result = Context.GetCodecInitializationResult();
                return result == CodecInitializationResult.Success;
            }
        }
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
    public delegate void DelegateNativeOnIceCandidate([MarshalAs(UnmanagedType.LPStr)] string candidate, [MarshalAs(UnmanagedType.LPStr)] string sdpMid, int sdpMlineIndex);
    [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
    public delegate void DelegateOnIceCandidate(RTCIceCandidate candidate);
    [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
    //according to JS API naming, use OnNegotiationNeeded instead of OnRenegotiationNeeded
    public delegate void DelegateOnNegotiationNeeded();
    [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
    public delegate void DelegateNativeOnTrack(IntPtr rtpTransceiverInterface);
    [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
    public delegate void DelegateOnTrack(RTCTrackEvent e);
    [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
    internal delegate void DelegatePeerConnectionCallbackEvent(RTCPeerConnectionEventType type, [MarshalAs(UnmanagedType.LPStr, SizeConst = 1024)] string str);
    [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
    internal delegate void DelegateNativeOnDataChannel(IntPtr ptr);
    [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
    internal delegate void DelegateNativeOnMessage([MarshalAs(UnmanagedType.LPArray, SizeParamIndex = 1)] byte[] bytes, int size);
    [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
    public delegate void DelegateOnOpen();
    [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
    public delegate void DelegateOnClose();

    internal static class NativeMethods
    {
        [DllImport(WebRTC.Lib)]
        public static extern void StopMediaStreamTrack(IntPtr context, IntPtr track);
        [DllImport(WebRTC.Lib)]
        public static extern CodecInitializationResult GetCodecInitializationResult();
        [DllImport(WebRTC.Lib)]
        public static extern void RegisterDebugLog(DelegateDebugLog func);
        [DllImport(WebRTC.Lib)]
        public static extern Context ContextCreate(int uid);
        [DllImport(WebRTC.Lib)]
        public static extern void ContextDestroy(int uid);
        [DllImport(WebRTC.Lib)]
        public static extern IntPtr ContextCreatePeerConnection(IntPtr ptr, int id);
        [DllImport(WebRTC.Lib)]
        public static extern IntPtr ContextCreatePeerConnectionWithConfig(IntPtr ptr, int id, string conf);
        [DllImport(WebRTC.Lib)]
        public static extern void ContextDeletePeerConnection(IntPtr ptr, int id);
        [DllImport(WebRTC.Lib)]
        public static extern void PeerConnectionClose(IntPtr ptr);
        [DllImport(WebRTC.Lib)]
        public static extern RTCErrorType PeerConnectionSetConfiguration(IntPtr ptr, [MarshalAs(UnmanagedType.LPStr, SizeConst = 256)] string conf);
        [DllImport(WebRTC.Lib)]
        public static extern IntPtr PeerConnectionCreateDataChannel(IntPtr ptr, [MarshalAs(UnmanagedType.LPStr, SizeConst = 256)] string label, ref RTCDataChannelInit options);
        [DllImport(WebRTC.Lib)]
        public static extern void PeerConnectionGetConfiguration(IntPtr ptr, ref IntPtr conf, ref int len);
        [DllImport(WebRTC.Lib)]
        public static extern void PeerConnectionCreateOffer(IntPtr ptr, ref RTCOfferOptions options);
        [DllImport(WebRTC.Lib)]
        public static extern void PeerConnectionCreateAnswer(IntPtr ptr, ref RTCAnswerOptions options);
        [DllImport(WebRTC.Lib)]
        public static extern void PeerConnectionRegisterCallbackCreateSD(IntPtr ptr, DelegateCreateSDSuccess onSuccess, DelegateCreateSDFailure onFailure);
        [DllImport(WebRTC.Lib)]
        public static extern void PeerConnectionRegisterCallbackSetSD(IntPtr ptr, DelegateSetSDSuccess onSuccess, DelegateSetSDFailure onFailure);
        [DllImport(WebRTC.Lib)]
        public static extern void PeerConnectionRegisterIceConnectionChange(IntPtr ptr, DelegateOnIceConnectionChange callback);
        [DllImport(WebRTC.Lib)]
        public static extern void PeerConnectionRegisterOnIceCandidate(IntPtr ptr, DelegateNativeOnIceCandidate callback);
        [DllImport(WebRTC.Lib)]
        public static extern void PeerConnectionSetLocalDescription(IntPtr ptr, ref RTCSessionDescription desc);
        [DllImport(WebRTC.Lib)]
        public static extern void PeerConnectionGetLocalDescription(IntPtr ptr, ref RTCSessionDescription desc);
        [DllImport(WebRTC.Lib)]
        public static extern void PeerConnectionSetRemoteDescription(IntPtr ptr, ref RTCSessionDescription desc);
        [DllImport(WebRTC.Lib)]
        public static extern IntPtr PeerConnectionAddTrack(IntPtr pc, IntPtr track);
        [DllImport(WebRTC.Lib)]
        public static extern void PeerConnectionRemoveTrack(IntPtr pc, IntPtr sender);
        [DllImport(WebRTC.Lib)]
        public static extern bool PeerConnectionAddIceCandidate(IntPtr ptr, ref RTCIceCandidate​ candidate);
        [DllImport(WebRTC.Lib)]
        public static extern RTCPeerConnectionState PeerConnectionState(IntPtr ptr);
        [DllImport(WebRTC.Lib)]
        public static extern RTCIceConnectionState PeerConnectionIceConditionState(IntPtr ptr);
        [DllImport(WebRTC.Lib)]
        public static extern void PeerConnectionRegisterOnDataChannel(IntPtr ptr, DelegateNativeOnDataChannel callback);
        [DllImport(WebRTC.Lib)]
        public static extern void PeerConnectionRegisterOnRenegotiationNeeded(IntPtr ptr, DelegateOnNegotiationNeeded callback);
        [DllImport(WebRTC.Lib)]
        public static extern void PeerConnectionRegisterOnTrack(IntPtr ptr, DelegateNativeOnTrack rtpTransceiverInterface);
        [DllImport(WebRTC.Lib)]
        public static extern IntPtr RtpTransceiverInterfaceGetTrack(IntPtr rtpTransceiverInterface);
        [DllImport(WebRTC.Lib)]
        public static extern int DataChannelGetID(IntPtr ptr);
        [DllImport(WebRTC.Lib)]
        public static extern IntPtr DataChannelGetLabel(IntPtr ptr);
        [DllImport(WebRTC.Lib)]
        public static extern void DataChannelSend(IntPtr ptr, [MarshalAs(UnmanagedType.LPStr)]string msg);
        [DllImport(WebRTC.Lib)]
        public static extern void DataChannelSendBinary(IntPtr ptr, [MarshalAs(UnmanagedType.LPArray, SizeParamIndex = 2)] byte[] bytes, int size);
        [DllImport(WebRTC.Lib)]
        public static extern void DataChannelClose(IntPtr ptr);
        [DllImport(WebRTC.Lib)]
        public static extern void DataChannelRegisterOnMessage(IntPtr ptr, DelegateNativeOnMessage callback);
        [DllImport(WebRTC.Lib)]
        public static extern void DataChannelRegisterOnOpen(IntPtr ptr, DelegateOnOpen callback);
        [DllImport(WebRTC.Lib)]
        public static extern void DataChannelRegisterOnClose(IntPtr ptr, DelegateOnClose callback);
        [DllImport(WebRTC.Lib)]
        public static extern IntPtr CaptureVideoStream(IntPtr context, IntPtr rt, int width, int height);
        [DllImport(WebRTC.Lib)]
        public static extern IntPtr CaptureAudioStream(IntPtr context);
        [DllImport(WebRTC.Lib)]
        public static extern void MediaStreamAddTrack(IntPtr stream, IntPtr track);
        [DllImport(WebRTC.Lib)]
        public static extern void MediaStreamRemoveTrack(IntPtr stream, IntPtr track);
        [DllImport(WebRTC.Lib)]
        public static extern IntPtr MediaStreamGetVideoTracks(IntPtr stream, ref int length);
        [DllImport(WebRTC.Lib)]
        public static extern IntPtr MediaStreamGetAudioTracks(IntPtr stream, ref int length);
        [DllImport(WebRTC.Lib)]
        public static extern IntPtr MediaStreamGetID(IntPtr stream);
        [DllImport(WebRTC.Lib)]
        public static extern TrackKind MediaStreamTrackGetKind(IntPtr track);
        [DllImport(WebRTC.Lib)]
        public static extern TrackState MediaStreamTrackGetReadyState(IntPtr track);
        [DllImport(WebRTC.Lib)]
        public static extern IntPtr MediaStreamTrackGetID(IntPtr track);
        [DllImport(WebRTC.Lib)]
        public static extern bool MediaStreamTrackGetEnabled(IntPtr track);
        [DllImport(WebRTC.Lib)]
        public static extern void MediaStreamTrackSetEnabled(IntPtr track, bool enabled);
        [DllImport(WebRTC.Lib)]
        public static extern void SetCurrentContext(IntPtr context);
        [DllImport(WebRTC.Lib)]
        public static extern IntPtr GetRenderEventFunc(IntPtr context);
        [DllImport(WebRTC.Lib)]
        public static extern void ProcessAudio(float[] data, int size);
    }

    internal struct Context
    {
        internal IntPtr self;
        public bool IsNull { get { return self == IntPtr.Zero; } }
        public static implicit operator bool(Context v) { return v.self != IntPtr.Zero; }
        public static bool ToBool(Context v) { return v; }
        public static Context Create(int uid = 0) { return NativeMethods.ContextCreate(uid); }
        public static CodecInitializationResult GetCodecInitializationResult() { return NativeMethods.GetCodecInitializationResult(); }
        public void Destroy(int uid = 0) { NativeMethods.ContextDestroy(uid); self = IntPtr.Zero; }
        public IntPtr CaptureVideoStream(IntPtr rt, int width, int height) { return NativeMethods.CaptureVideoStream(self, rt, width, height); }
        public IntPtr CaptureAudioStream() { return NativeMethods.CaptureAudioStream(self); }
        public IntPtr GetRenderEventFunc() { return NativeMethods.GetRenderEventFunc(self); }
        public void StopMediaStreamTrack(IntPtr track) { NativeMethods.StopMediaStreamTrack(self, track); }
    }
}


