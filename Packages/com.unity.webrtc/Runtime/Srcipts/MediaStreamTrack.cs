using System.Collections;
using System.Collections.Generic;
using System;
using System.Runtime.InteropServices;

namespace Unity.WebRTC
{
    public class MediaStreamTrack
    {
        internal IntPtr self;
        private TrackKind kind;
        private string id;
        private bool enabled;
        private TrackState readyState;

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
            Kind = NativeMethods.MediaStreamTrackGetKind(self);
            Id = Marshal.PtrToStringAnsi(NativeMethods.MediaStreamTrackGetID(self));
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

        public MediaStreamTrack Track { get => track; set => track = value; }
        internal RTCRtpSender(IntPtr ptr)
        {
            self = ptr;
        }
    }
    public class RTCTrackEvent
    {
        private IntPtr self;
        private MediaStreamTrack track;

        public MediaStreamTrack Track { get => track; set => track = value; }
        internal RTCTrackEvent(IntPtr ptr)
        {
            self = ptr;
        }
    }

}

