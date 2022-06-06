using System;
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
        /// <param name="index"></param>
        /// <exception cref="ArgumentOutOfRangeException"></exception>
        public override void FilterCodecs(int index)
        {
            if (index < 0)
            {
                m_codecs.Clear();
                return;
            }

            if (!AvailableCodecsUtils.TryGetAvailableAudioCodec(index, out var codec))
            {
                throw new ArgumentOutOfRangeException(nameof(index), index, "Index was out of range.");
            }

            m_codecs.Clear();
            m_codecs.Add(codec);
        }

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
