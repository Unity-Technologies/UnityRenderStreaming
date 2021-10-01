using Unity.WebRTC;
using UnityEngine;

namespace Unity.RenderStreaming
{
    /// <summary>
    /// Attach AudioListerner or AudioSource
    /// </summary>
    public class AudioStreamer : AudioStreamBase
    {
        [SerializeField]
        private AudioSource audioSource;

        private AudioStreamTrack track;
        int _sampleRate = 0;

        public AudioSource AudioSource
        {
            set
            {
                audioSource = value;
                _sampleRate = audioSource.clip.samples;
            }
            get
            {
                return audioSource;
            }
        }

        protected virtual void Awake()
        {
            if(audioSource != null && audioSource.clip != null)
            {
                _sampleRate = audioSource.clip.samples;
            }
            else
            {
                _sampleRate = AudioSettings.outputSampleRate;
            }
        }

        protected override MediaStreamTrack CreateTrack()
        {
            track = new AudioStreamTrack();
            return track;
        }

        protected virtual void OnEnable()
        {
            if (track != null)
                track.Enabled = true;
        }

        protected virtual void OnDisable()
        {
            if (track != null)
                track.Enabled = false;
        }

        private void OnAudioFilterRead(float[] data, int channels)
        {
            track?.SetData(data, channels, _sampleRate);
        }
    }
}
