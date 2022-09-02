using System;
using System.Linq;
using System.Collections.Generic;
using Unity.Collections;
using Unity.WebRTC;
using UnityEngine;

namespace Unity.RenderStreaming
{
    /// <summary>
    /// 
    /// </summary>
    public enum AudioStreamSource
    {
        /// <summary>
        /// 
        /// </summary>
        AudioListener = 0,
        /// <summary>
        /// 
        /// </summary>
        AudioSource = 1
    }

    /// <summary>
    /// Attach AudioListerner or AudioSource
    /// </summary>
    [AddComponentMenu("Render Streaming/Audio Stream Sender")]
    public class AudioStreamSender : StreamSenderBase
    {
        static readonly uint s_defaultMinBitrate = 0;
        static readonly uint s_defaultMaxBitrate = 200;

        [SerializeField]
        private AudioStreamSource m_Source;

        [SerializeField]
        private AudioListener m_AudioListener;

        [SerializeField]
        private AudioSource m_AudioSource;

        [SerializeField, Codec]
        private AudioCodecInfo m_Codec;

        [SerializeField, Bitrate(0, 1000)]
        private Range m_Bitrate = new Range(s_defaultMinBitrate, s_defaultMaxBitrate);

        protected AudioStreamTrack track;

        private int m_sampleRate = 0;

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
        public uint minBitrate
        {
            get { return m_Bitrate.min; }
        }

        /// <summary>
        /// 
        /// </summary>
        public uint maxBitrate
        {
            get { return m_Bitrate.max; }
        }

        /// <summary>
        /// 
        /// </summary>
        /// <returns></returns>
        static public IEnumerable<AudioCodecInfo> GetAvailableCodecs()
        {
            var excludeCodecMimeType = new[] { "audio/CN", "audio/telephone-event" };
            var capabilities = RTCRtpSender.GetCapabilities(TrackKind.Audio);
            return capabilities.codecs.Where(codec => !excludeCodecMimeType.Contains(codec.mimeType)).Select(codec => new AudioCodecInfo(codec));
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="minBitrate"></param>
        /// <param name="maxBitrate"></param>
        public void SetBitrate(uint minBitrate, uint maxBitrate)
        {
            if (minBitrate > maxBitrate)
                throw new ArgumentException("The maxBitrate must be greater than minBitrate.", "maxBitrate");
            m_Bitrate.min = minBitrate;
            m_Bitrate.max = maxBitrate;
            foreach (var transceiver in Transceivers.Values)
            {
                RTCError error = transceiver.Sender.SetBitrate(m_Bitrate.min, m_Bitrate.max);
                if (error.errorType != RTCErrorType.None)
                    Debug.LogError(error.message);
            }
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="codec"></param>
        public void SetCodec(AudioCodecInfo codec)
        {
            m_Codec = codec;
            foreach (var transceiver in Transceivers.Values)
            {
                if (!string.IsNullOrEmpty(transceiver.Mid))
                    continue;
                if (transceiver.Sender.Track.ReadyState == TrackState.Ended)
                    continue;

                var codecs = new AudioCodecInfo[] { m_Codec };
                RTCErrorType error = transceiver.SetCodecPreferences(SelectCodecCapabilities(codecs).ToArray());
                if (error != RTCErrorType.None)
                    throw new InvalidOperationException($"Set codec is failed. errorCode={error}");
            }
        }

        internal IEnumerable<RTCRtpCodecCapability> SelectCodecCapabilities(IEnumerable<AudioCodecInfo> codecs)
        {
            return RTCRtpSender.GetCapabilities(TrackKind.Audio).SelectCodecCapabilities(codecs);
        }

        protected virtual void Awake()
        {
            OnStartedStream += _OnStartedStream;
            OnStoppedStream += _OnStoppedStream;
        }

        void OnAudioConfigurationChanged(bool deviceWasChanged)
        {
            m_sampleRate = AudioSettings.outputSampleRate;
        }

        void _OnStartedStream(string connectionId)
        {
        }

        void _OnStoppedStream(string connectionId)
        {
            track = null;
        }

        internal override MediaStreamTrack CreateTrack()
        {
            switch(m_Source)
            {
                case AudioStreamSource.AudioListener:
                    // todo: Should add AudioStreamTrack supports AudioListener
                    if (!GetComponent<AudioListener>())
                        throw new InvalidOperationException("Audio Listener have to be set the same gameObject.");
                    return new AudioStreamTrack();
                case AudioStreamSource.AudioSource:
                    return new AudioStreamTrack(m_AudioSource);
            }
            throw new InvalidOperationException("");
        }

        protected override void OnEnable()
        {
            OnAudioConfigurationChanged(false);
            AudioSettings.OnAudioConfigurationChanged += OnAudioConfigurationChanged;

            if (track != null)
                track.Enabled = true;
        }

        protected override void OnDisable()
        {
            AudioSettings.OnAudioConfigurationChanged -= OnAudioConfigurationChanged;

            try
            {
                if (track != null)
                    track.Enabled = false;
            }
            catch (InvalidOperationException)
            {
                track = null;
            }
        }

        protected virtual void OnAudioFilterRead(float[] data, int channels)
        {
            // todo: Should add AudioStreamTrack supports AudioListener
            if (m_Source != AudioStreamSource.AudioListener)
                return;

            NativeArray<float> nativeArray = new NativeArray<float>(data, Allocator.Temp);
            try
            {
                track?.SetData(ref nativeArray, channels, m_sampleRate);
            }
            // todo(kazuki):: Should catch only ObjectDisposedException but
            // AudioStreamTrack also throws NullReferenceException.
            catch (Exception)
            {
                track = null;
            }
            finally
            {
                nativeArray.Dispose();
            }
        }
    }
}
