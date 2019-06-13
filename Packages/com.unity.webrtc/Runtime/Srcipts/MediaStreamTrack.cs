using System;
using System.Runtime.InteropServices;
using UnityEngine;

namespace Unity.WebRTC
{
    public class MediaStreamTrack
    {
        internal IntPtr self;
        private TrackKind kind;
        private string id;
        private bool enabled;
        private TrackState readyState;
        private RenderTexture rt;

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

        public RenderTexture Rt { get => rt; private set => rt = value; }

        internal MediaStreamTrack(RenderTexture rt, IntPtr ptr)
        {
            self = ptr;
            kind = NativeMethods.MediaStreamTrackGetKind(self);
            id = Marshal.PtrToStringAnsi(NativeMethods.MediaStreamTrackGetID(self));
            Rt = rt;
        }
        internal MediaStreamTrack(IntPtr ptr) 
        {
            self = ptr;
            kind = NativeMethods.MediaStreamTrackGetKind(self);
            id = Marshal.PtrToStringAnsi(NativeMethods.MediaStreamTrackGetID(self));
        }
        //Disassociate track from its source(video or audio), not for destroying the track
        public void Stop()
        {
            if (kind == TrackKind.Video)
            {
                NativeMethods.StopMediaStreamTrack(self);
                if (Rt != null)
                {
                    CameraExtension.RemoveRt(Rt);
                    Rt.Release();
                    UnityEngine.Object.Destroy(Rt);
                }
            }
            else
            {
                Audio.Stop();
            }
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

