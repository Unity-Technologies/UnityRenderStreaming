using Unity.WebRTC;
using UnityEngine;

namespace Unity.RenderStreaming
{
    /// <summary>
    ///
    /// </summary>
    public class AudioStreamReceiver : StreamReceiverBase
    {
        /// <summary>
        ///
        /// </summary>
        /// <param name="clip"></param>
        public delegate void OnUpdateReceiveAudioSourceHandler(AudioSource source);

        /// <summary>
        ///
        /// </summary>
        public OnUpdateReceiveAudioSourceHandler OnUpdateReceiveAudioSource;

        /// <summary>
        ///
        /// </summary>
        public override TrackKind Kind { get { return TrackKind.Audio; } }

        /// <summary>
        ///
        /// </summary>
        public AudioSource Source => m_source;

        private AudioSource m_source;

        /// <summary>
        ///
        /// </summary>
        /// <param name="source"></param>
        public void SetSource(AudioSource source)
        {
            m_source = source;
        }

        protected virtual void Start()
        {
            OnStartedStream += StartedStream;
            OnStoppedStream += StoppedStream;
        }

        private void StartedStream(string connectionId)
        {
            if (Track is AudioStreamTrack audioTrack)
            {
                m_source?.SetTrack(audioTrack);
                OnUpdateReceiveAudioSource?.Invoke(m_source);
            }
        }

        private void StoppedStream(string connectionId)
        {
            m_source = null;
        }
    }
}
