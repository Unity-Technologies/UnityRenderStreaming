using System;
using Unity.WebRTC;
using UnityEngine;

namespace Unity.RenderStreaming
{
    [RequireComponent(typeof(AudioSource))]
    public class AudioStreamer : AudioStreamBase
    {
        public AudioSource audioSource;

        private AudioStreamTrack track;

        protected override MediaStreamTrack CreateTrack()
        {
            track = new AudioStreamTrack(audioSource);
            return track;
        }

        void OnAudioFilterRead(float[] data, int channels)
        {
            track.OnAudioRead(data, channels);
        }
    }
}
