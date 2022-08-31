using System;
using System.Linq;
using System.Collections.Generic;
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
        public AudioCodecInfo codec
        {
            get { return m_codec; }
        }

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

        private AudioCodecInfo m_codec;

        /// <summary>
        /// 
        /// </summary>
        /// <returns></returns>
        static public IEnumerable<AudioCodecInfo> GetAvailableCodecs()
        {
            var excludeCodecMimeType = new[] { "audio/CN", "audio/telephone-event" };
            var capabilities = RTCRtpReceiver.GetCapabilities(TrackKind.Audio);
            return capabilities.codecs.Where(codec => !excludeCodecMimeType.Contains(codec.mimeType)).Select(codec => new AudioCodecInfo(codec));
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="mimeType"></param>
        public void SetCodec(AudioCodecInfo codec)
        {
            m_codec = codec;

            if (Transceiver == null)
                return;
            if (!string.IsNullOrEmpty(Transceiver.Mid))
                throw new InvalidOperationException("Transceiver is streaming. This operation is invalid during the track is in use.");
            if (Transceiver.Sender.Track.ReadyState == TrackState.Ended)
                throw new InvalidOperationException("Track has already been ended.");

            var codecs = new AudioCodecInfo[] { m_codec };
            RTCErrorType error = Transceiver.SetCodecPreferences(SelectCodecCapabilities(codecs).ToArray());
            if (error != RTCErrorType.None)
                throw new InvalidOperationException($"Set codec is failed. errorCode={error}");
        }

        /// <summary>
        ///
        /// </summary>
        /// <param name="source"></param>
        public void SetSource(AudioSource source)
        {
            m_source = source;
        }

        internal IEnumerable<RTCRtpCodecCapability> SelectCodecCapabilities(IEnumerable<AudioCodecInfo> codecs)
        {
            return RTCRtpReceiver.GetCapabilities(TrackKind.Audio).SelectCodecCapabilities(codecs);
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
