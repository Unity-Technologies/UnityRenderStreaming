using System;
using System.Linq;
using System.Collections.Generic;
using Unity.Collections;
using Unity.WebRTC;
using UnityEngine;

namespace Unity.RenderStreaming
{
    /// <summary>
    /// Attach AudioListerner or AudioSource
    /// </summary>
    public class AudioStreamSender : StreamSenderBase
    {
        const uint s_defaultMinBitrate = 0;
        const uint s_defaultMaxBitrate = 200;

        [SerializeField, Bitrate(0, 1000)]
        private Range m_bitrate = new Range(s_defaultMinBitrate, s_defaultMaxBitrate);


        private AudioCodecInfo m_codec;

        protected AudioStreamTrack track;

        private int m_sampleRate = 0;

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
        public uint minBitrate
        {
            get { return m_bitrate.min; }
        }

        /// <summary>
        /// 
        /// </summary>
        public uint maxBitrate
        {
            get { return m_bitrate.max; }
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
        /// <param name="bitrate"></param>
        public void SetBitrate(uint minBitrate, uint maxBitrate)
        {
            if (minBitrate > maxBitrate)
                throw new ArgumentException("The maxBitrate must be greater than minBitrate.", "maxBitrate");
            m_bitrate.min = minBitrate;
            m_bitrate.max = maxBitrate;
            foreach (var transceiver in Transceivers.Values)
            {
                RTCError error = transceiver.Sender.SetBitrate(m_bitrate.min, m_bitrate.max);
                if (error.errorType != RTCErrorType.None)
                    Debug.LogError(error.message);
            }
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="mimeType"></param>
        public void SetCodec(AudioCodecInfo codec)
        {
            m_codec = codec;

            foreach (var transceiver in Transceivers.Values)
            {
                if (!string.IsNullOrEmpty(transceiver.Mid))
                    continue;
                if (transceiver.Sender.Track.ReadyState == TrackState.Ended)
                    continue;

                RTCErrorType error = transceiver.SetCodec(new AudioCodecInfo[] { m_codec });
                if (error != RTCErrorType.None)
                    throw new InvalidOperationException($"Set codec is failed. errorCode={error}");
            }
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

        protected override MediaStreamTrack CreateTrack()
        {
            track = new AudioStreamTrack();
            return track;
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
