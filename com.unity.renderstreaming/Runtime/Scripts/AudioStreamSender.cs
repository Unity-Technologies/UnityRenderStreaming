using System;
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
        private uint m_bitrate = s_defaultBitrate;

        /// <summary>
        /// 
        /// </summary>
        public uint bitrate
        {
            get { return m_bitrate; }
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="bitrate"></param>
        public void SetBitrate(uint bitrate)
        {
            if (bitrate < 0)
                throw new ArgumentException();
            m_bitrate = bitrate;
            foreach (var transceiver in Transceivers.Values)
            {
                RTCError error = transceiver.Sender.SetBitrate(m_bitrate);
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
