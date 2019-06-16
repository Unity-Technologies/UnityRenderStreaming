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
        private RenderTexture[] rts;

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

        public RenderTexture[] Rts { get => rts; private set => rts = value; }

        internal MediaStreamTrack(RenderTexture[] rt, IntPtr ptr)
        {
            self = ptr;
            kind = NativeMethods.MediaStreamTrackGetKind(self);
            id = Marshal.PtrToStringAnsi(NativeMethods.MediaStreamTrackGetID(self));
            Rts = rts;
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
                if (Rts != null)
                {
                    CameraExtension.RemoveRt(Rts);
                    Rts[0].Release();
                    Rts[1].Release();
                    UnityEngine.Object.Destroy(Rts[0]);
                    UnityEngine.Object.Destroy(Rts[1]);
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

