using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System;

namespace Unity.WebRTC
{
    public class MediaStream
    {
        private IntPtr self;
        private string id;
        public MediaStreamTrack[] GetTracks()
        {
            MediaStreamTrack[] tmpTracks = new MediaStreamTrack[0] { };
            return tmpTracks;
        }
        public MediaStreamTrack[] GetAudioTracks()
        {
            MediaStreamTrack[] tmpTracks = new MediaStreamTrack[0] { };
            return tmpTracks;
        }
        public MediaStreamTrack[] GetVideoTracks()
        {
            MediaStreamTrack[] tmpTracks = new MediaStreamTrack[0] { };
            return tmpTracks;
        }

        public void AddTrack(MediaStreamTrack track)
        {

        }
        public void RemoveTrack(MediaStreamTrack track)
        {

        }



        public string Id { get => id; set => id = value; }
    }

    public static class CameraExtension
    {
        public static MediaStream CaptureStream(this Camera cam)
        {
            return new MediaStream();
        }
    }

    public static class Audio
    {
        public static MediaStream CaptureStream()
        {
            return new MediaStream();
        }
    }
}
