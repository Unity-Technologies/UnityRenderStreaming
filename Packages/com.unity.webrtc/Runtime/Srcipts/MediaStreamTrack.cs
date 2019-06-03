using System.Collections;
using System.Collections.Generic;
using System;
using System.Runtime.InteropServices;

namespace Unity.WebRTC
{
    public class MediaStreamTrack
    {
        private IntPtr self;
        private TrackKind kind;
        private string id;
        private bool enabled;
        private TrackState readyState;



        public TrackKind Kind { get => kind; set => kind = value; }
        public string Id { get => id; set => id = value; }
        public bool Enabled { get => enabled; set => enabled = value; }
        public TrackState ReadyState { get => readyState; set => readyState = value; }
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
        private IntPtr self;
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

