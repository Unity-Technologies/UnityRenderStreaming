using System;
using System.Collections.Generic;
using System.Linq;
using Unity.WebRTC;
using UnityEngine;

namespace Unity.RenderStreaming
{
    /// <summary>
    ///
    /// </summary>
    [AddComponentMenu("Render Streaming/Audio Stream Receiver")]
    public class AudioStreamReceiver : StreamReceiverBase
    {
        internal const string CodecPropertyName = nameof(m_Codec);
        internal const string TargetAudioSourcePropertyName = nameof(m_TargetAudioSource);

        /// <summary>
        /// 
        /// </summary>
        /// <param name="source"></param>
        public delegate void OnUpdateReceiveAudioSourceHandler(AudioSource source);

        /// <summary>
        ///
        /// </summary>
        public OnUpdateReceiveAudioSourceHandler OnUpdateReceiveAudioSource;

        [SerializeField]
        private AudioSource m_TargetAudioSource;

        [SerializeField, Codec]
        private AudioCodecInfo m_Codec;

        /// <summary>
        /// 
        /// </summary>
        public AudioCodecInfo codec
        {
            get { return m_Codec; }
        }

        /// <summary>
        ///
        /// </summary>
        public AudioSource targetAudioSource
        {
            get { return m_TargetAudioSource; }
            set { m_TargetAudioSource = value; }
        }

        /// <summary>
        /// 
        /// </summary>
        /// <returns></returns>
        static public IEnumerable<AudioCodecInfo> GetAvailableCodecs()
        {
            var excludeCodecMimeType = new[] { "audio/CN", "audio/telephone-event" };
            var capabilities = RTCRtpReceiver.GetCapabilities(TrackKind.Audio);
            return capabilities.codecs.Where(codec => !excludeCodecMimeType.Contains(codec.mimeType)).Select(codec => AudioCodecInfo.Create(codec));
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="mimeType"></param>
        public void SetCodec(AudioCodecInfo codec)
        {
            m_Codec = codec;

            if (Transceiver == null)
                return;
            if (!string.IsNullOrEmpty(Transceiver.Mid))
                throw new InvalidOperationException("Transceiver is streaming. This operation is invalid during the track is in use.");
            if (Transceiver.Sender.Track.ReadyState == TrackState.Ended)
                throw new InvalidOperationException("Track has already been ended.");

            var codecs = new AudioCodecInfo[] { m_Codec };
            RTCErrorType error = Transceiver.SetCodecPreferences(SelectCodecCapabilities(codecs).ToArray());
            if (error != RTCErrorType.None)
                throw new InvalidOperationException($"Set codec is failed. errorCode={error}");
        }

        internal IEnumerable<RTCRtpCodecCapability> SelectCodecCapabilities(IEnumerable<AudioCodecInfo> codecs)
        {
            return RTCRtpReceiver.GetCapabilities(TrackKind.Audio).SelectCodecCapabilities(codecs);
        }

        private protected virtual void Start()
        {
            OnStartedStream += StartedStream;
            OnStoppedStream += StoppedStream;
        }

        private void StartedStream(string connectionId)
        {
            if (Track is AudioStreamTrack audioTrack)
            {
                m_TargetAudioSource?.SetTrack(audioTrack);
                OnUpdateReceiveAudioSource?.Invoke(m_TargetAudioSource);
            }
        }

        private void StoppedStream(string connectionId)
        {
        }
    }
}
