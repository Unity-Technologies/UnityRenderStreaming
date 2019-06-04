﻿using UnityEngine;
using System;
using System.Runtime.InteropServices;

namespace Unity.WebRTC
{
    public class MediaStream
    {
        private IntPtr self;
        private string id;

        public string Id { get => id; private set { } }

        public MediaStreamTrack[] GetTracks()
        {
            int audioTrackSize = 0, videoTrackSize = 0;
            IntPtr audioPtr = NativeMethods.MediaStreamGetAudioTracks(self, ref audioTrackSize);
            IntPtr videoPtr = NativeMethods.MediaStreamGetVideoTracks(self, ref videoTrackSize);
            IntPtr[] tracksPtr = new IntPtr[audioTrackSize + videoTrackSize];
            Marshal.Copy(audioPtr, tracksPtr, 0, audioTrackSize);
            Marshal.Copy(videoPtr, tracksPtr, audioTrackSize, videoTrackSize);
            Marshal.FreeCoTaskMem(audioPtr);
            Marshal.FreeCoTaskMem(videoPtr);

            MediaStreamTrack[] tracks = new MediaStreamTrack[audioTrackSize + videoTrackSize];
            for (int i = 0; i < audioTrackSize + videoTrackSize; i++)
            {
                tracks[i] = new MediaStreamTrack(tracksPtr[i]);
            }
            return tracks;
        }
        public MediaStreamTrack[] GetAudioTracks()
        {
            int trackSize = 0;
            IntPtr ptr = NativeMethods.MediaStreamGetAudioTracks(self, ref trackSize);
            IntPtr[] tracksPtr = new IntPtr[trackSize];
            Marshal.Copy(ptr, tracksPtr, 0, trackSize);
            Marshal.FreeCoTaskMem(ptr);

            MediaStreamTrack[] tracks = new MediaStreamTrack[trackSize];
            for (int i = 0; i < trackSize; i++)
            {
                tracks[i] = new MediaStreamTrack(tracksPtr[i]);
            }
            return tracks;
        }
        public MediaStreamTrack[] GetVideoTracks()
        {
            int trackSize = 0;
            IntPtr ptr = NativeMethods.MediaStreamGetVideoTracks(self, ref trackSize);
            IntPtr[] tracksPtr = new IntPtr[trackSize];
            Marshal.Copy(ptr, tracksPtr, 0, trackSize);
            Marshal.FreeCoTaskMem(ptr);

            MediaStreamTrack[] tracks = new MediaStreamTrack[trackSize];
            for (int i = 0; i < trackSize; i++)
            {
                tracks[i] = new MediaStreamTrack(tracksPtr[i]);
            }
            return tracks;
        }

        public void AddTrack(MediaStreamTrack track)
        {
            NativeMethods.MediaStreamAddTrack(self, track.self);
        }
        public void RemoveTrack(MediaStreamTrack track)
        {
            NativeMethods.MediaStreamRemoveTrack(self, track.self);
        }
        internal MediaStream(IntPtr ptr)
        {
            self = ptr;
            Id = Marshal.PtrToStringAnsi(NativeMethods.MeidaStreamGetID(self));
        }

    }

    public static class CameraExtension
    {
        public static MediaStream CaptureStream(this Camera cam)
        {
            return new MediaStream(WebRTC.Context.CaptureVideoStream());
        }
    }

    public static class Audio
    {
        public static MediaStream CaptureStream()
        {
            return new MediaStream(WebRTC.Context.CaptureAudioStream());
        }
    }
}
