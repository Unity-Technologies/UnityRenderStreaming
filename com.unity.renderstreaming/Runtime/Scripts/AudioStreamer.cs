using Unity.WebRTC;
using UnityEngine;

namespace Unity.RenderStreaming
{
    /// <summary>
    /// Attach AudioListerner or AudioSource
    /// </summary>
    public class AudioStreamer : AudioStreamBase
    {
        public AudioSource audioSource;
        public int sampleRate = 48000;

        private AudioStreamTrack track;

        protected override MediaStreamTrack CreateTrack()
        {
            track = new AudioStreamTrack();
            return track;
        }

        protected virtual void OnDestroy()
        {
            track?.Dispose();
        }

        protected virtual void OnEnable()
        {
            if(track != null)
                track.Enabled = true;
        }

        protected virtual void OnDisable()
        {
            if (track != null)
                track.Enabled = false;
        }

        private void OnAudioFilterRead(float[] data, int channels)
        {
            track?.SetData(data, channels, sampleRate);
        }
    }
}
