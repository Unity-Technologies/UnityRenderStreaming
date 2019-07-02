using System;
using System.Runtime.InteropServices;
using UnityEngine;

namespace Unity.WebRTC
{
    public class MediaStreamTrack
    {
        internal IntPtr nativePtr;
        protected string id;
        protected TrackKind kind;
        public MediaStreamTrack()
        {

        }

        internal MediaStreamTrack(IntPtr ptr)
        {
            nativePtr = ptr;
            kind = NativeMethods.MediaStreamTrackGetKind(nativePtr);
            id = Marshal.PtrToStringAnsi(NativeMethods.MediaStreamTrackGetID(nativePtr));
        }

        public bool Enabled
        {
            get { return NativeMethods.MediaStreamTrackGetEnabled(nativePtr); }
            set { NativeMethods.MediaStreamTrackSetEnabled(nativePtr, value); }
        }

        public TrackState ReadyState
        {
            get
            { return NativeMethods.MediaStreamTrackGetReadyState(nativePtr); }
            private set { }
        }

        public TrackKind Kind { get => kind; private set { } }
        public string Id { get => id; private set { } }
    }

    public class VideoStreamTrack : MediaStreamTrack
    {
        public VideoStreamTrack(RenderTexture rt) : base()
        {
            IntPtr nativeVideoStreamPtr = WebRTC.Context.CaptureVideoStream(rt.GetNativeTexturePtr(), rt.width, rt.height);
            string nativeVideoStreamID = Marshal.PtrToStringAnsi(NativeMethods.MediaStreamGetID(nativeVideoStreamPtr));

            int trackSize = 0;
            IntPtr tracksNativePtr = NativeMethods.MediaStreamGetVideoTracks(nativeVideoStreamPtr, ref trackSize);
            IntPtr[] tracksPtr = new IntPtr[trackSize];
            Marshal.Copy(tracksNativePtr, tracksPtr, 0, trackSize);
            Marshal.FreeCoTaskMem(tracksNativePtr);

            nativePtr = tracksPtr[0];
            kind = NativeMethods.MediaStreamTrackGetKind(nativePtr);
            id = Marshal.PtrToStringAnsi(NativeMethods.MediaStreamTrackGetID(nativePtr));

        }
    }

    public class AudioStreamTrack : MediaStreamTrack
    {
        public AudioStreamTrack() : base()
        {
            IntPtr nativeAudioStreamPtr = WebRTC.Context.CaptureAudioStream();
            string nativeAudioStreamID = Marshal.PtrToStringAnsi(NativeMethods.MediaStreamGetID(nativeAudioStreamPtr));

            int trackSize = 0;
            IntPtr trackNativePtr = NativeMethods.MediaStreamGetAudioTracks(nativeAudioStreamPtr, ref trackSize);
            IntPtr[] tracksPtr = new IntPtr[trackSize];
            Marshal.Copy(trackNativePtr, tracksPtr, 0, trackSize);
            //TODO: Linux compatibility 
            Marshal.FreeCoTaskMem(trackNativePtr);

            nativePtr = tracksPtr[0];
            kind = NativeMethods.MediaStreamTrackGetKind(nativePtr);
            id = Marshal.PtrToStringAnsi(NativeMethods.MediaStreamTrackGetID(nativePtr));
        }
    }


    public enum TrackKind
    {
        Audio,
        Video
    }
    public enum TrackState
    {
        Live,
        Ended
    }
    public class RTCRtpSender
    {
        internal IntPtr self;
        private MediaStreamTrack track;

        internal RTCRtpSender(IntPtr ptr)
        {
            self = ptr;
        }
    }
    public class RTCTrackEvent
    {
        private IntPtr self;
        private MediaStreamTrack track;

        public MediaStreamTrack Track
        {
            get => new MediaStreamTrack(NativeMethods.RtpTransceiverInterfaceGetTrack(self));
            private set { }
        }
        internal RTCTrackEvent(IntPtr ptr)
        {
            self = ptr;
        }
    }

}

