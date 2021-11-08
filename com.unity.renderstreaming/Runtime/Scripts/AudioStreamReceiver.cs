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
        public delegate void OnUpdateReceiveAudioClipHandler(AudioClip clip);

        /// <summary>
        ///
        /// </summary>
        public OnUpdateReceiveAudioClipHandler OnUpdateReceiveAudioClip;

        /// <summary>
        ///
        /// </summary>
        public override TrackKind Kind { get { return TrackKind.Audio; } }

        /// <summary>
        ///
        /// </summary>
        public AudioClip Clip => m_renderer;

        private AudioClip m_renderer;

        protected virtual void Start()
        {
            OnStartedStream += StartedStream;
            OnStoppedStream += StoppedStream;
        }

        private void StartedStream(string connectionId)
        {
            var audioTrack = Track as AudioStreamTrack;
            audioTrack.OnAudioReceived += OnAudioReceived;
        }

        private void StoppedStream(string connectionId)
        {
            m_renderer = null;
            OnUpdateReceiveAudioClip?.Invoke(null);
        }

        private void OnAudioReceived(AudioClip clip)
        {
            m_renderer = clip;
            OnUpdateReceiveAudioClip?.Invoke(clip);
        }
    }
}
