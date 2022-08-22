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
        // todo(kazuki): check default value.
        const uint s_defaultBitrate = 1000;

        [SerializeField]
        private uint m_minBitrate = s_defaultBitrate;

        [SerializeField]
        private uint m_maxBitrate = s_defaultBitrate;

        /// <summary>
        /// 
        /// </summary>
        public uint minBitrate
        {
            get { return m_minBitrate; }
        }

        /// <summary>
        /// 
        /// </summary>
        public uint maxBitrate
        {
            get { return m_maxBitrate; }
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
            m_minBitrate = minBitrate;
            m_maxBitrate = maxBitrate;
            foreach (var transceiver in Transceivers.Values)
            {
                RTCError error = transceiver.Sender.SetBitrate(m_minBitrate, m_maxBitrate);
                if (error.errorType == RTCErrorType.None)
                    Debug.LogError(error.message);
            }
        }

        protected AudioStreamTrack track;

        int m_sampleRate = 0;


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

        protected virtual void OnEnable()
        {
            OnAudioConfigurationChanged(false);
            AudioSettings.OnAudioConfigurationChanged += OnAudioConfigurationChanged;

            if (track != null)
                track.Enabled = true;
        }

        protected virtual void OnDisable()
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
