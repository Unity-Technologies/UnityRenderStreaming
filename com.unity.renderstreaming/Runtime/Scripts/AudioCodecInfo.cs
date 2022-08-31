using System;
using UnityEngine;
using Unity.WebRTC;

namespace Unity.RenderStreaming
{
    /// <summary>
    /// 
    /// </summary>
    [Serializable]
    public class AudioCodecInfo : IEquatable<AudioCodecInfo>
    {
        [SerializeField]
        private string m_MimeType;
        [SerializeField]
        private string m_SdpFmtpLine;
        [SerializeField]
        private int m_ChannelCount;
        [SerializeField]
        private int m_SampleRate;

        /// <summary>
        /// 
        /// </summary>
        public string name { get { return m_MimeType.Split('/')[1]; } }

        /// <summary>
        /// 
        /// </summary>
        public string mimeType { get { return m_MimeType; } }

        /// <summary>
        /// 
        /// </summary>
        public int channelCount { get { return m_ChannelCount; } }

        /// <summary>
        /// 
        /// </summary>
        public int sampleRate { get { return m_SampleRate; } }

        /// <summary>
        /// 
        /// </summary>
        public string sdpFmtpLine { get { return m_SdpFmtpLine; } }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="other"></param>
        /// <returns></returns>
        public bool Equals(AudioCodecInfo other)
        {
            if (other == null)
                return false;
            return this.mimeType == other.mimeType
                && this.sdpFmtpLine == other.sdpFmtpLine
                && this.channelCount == other.channelCount
                && this.sampleRate == other.sampleRate;
        }

        internal bool Equals(RTCRtpCodecCapability other)
        {
            if (other == null)
                return false;
            return this.mimeType == other.mimeType
                && this.sdpFmtpLine == other.sdpFmtpLine
                && this.channelCount == other.channels
                && this.sampleRate == other.clockRate;
        }

        internal AudioCodecInfo(RTCRtpCodecCapability cap)
        {
            m_MimeType = cap.mimeType;
            m_SdpFmtpLine = cap.sdpFmtpLine;
            m_ChannelCount = cap.channels.GetValueOrDefault();
            m_SampleRate = cap.clockRate.GetValueOrDefault();
        }
    }
}
