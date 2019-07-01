using System;
using System.Runtime.InteropServices;
using UnityEngine;

namespace Unity.WebRTC
{
    public class MediaStreamTrack2
    {
        public IntPtr nativePtr;
        public string id;
        public TrackKind kind;
        public MediaStreamTrack2()
        {

        }
    }

    public class VideoStreamTrack : MediaStreamTrack2
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




    public class MediaStreamTrack
    {
        internal IntPtr self;
        private TrackKind kind;
        private string id;
        private bool enabled;
        private TrackState readyState;
        internal Action<MediaStreamTrack> stopTrack;
        internal Func<MediaStreamTrack, RenderTexture[]> getRts;

        public bool Enabled
        {
            get
            {
                return NativeMethods.MediaStreamTrackGetEnabled(self);
            }
            set
            {
                NativeMethods.MediaStreamTrackSetEnabled(self, value);
            }
        }
        public TrackState ReadyState
        {
            get
            {
                return NativeMethods.MediaStreamTrackGetReadyState(self);
            }
            private set { }
        }

        public TrackKind Kind { get => kind; private set { } }
        public string Id { get => id; private set { } }

        internal MediaStreamTrack(IntPtr ptr)
        {
            self = ptr;
            kind = NativeMethods.MediaStreamTrackGetKind(self);
            id = Marshal.PtrToStringAnsi(NativeMethods.MediaStreamTrackGetID(self));
        }
        //Disassociate track from its source(video or audio), not for destroying the track
        public void Stop()
        {
            stopTrack(this);
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

