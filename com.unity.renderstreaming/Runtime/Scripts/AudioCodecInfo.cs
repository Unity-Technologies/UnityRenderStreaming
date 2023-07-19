using System;
using Unity.WebRTC;
using UnityEngine;

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
        public string name { get { return m_MimeType.GetCodecName(); } }

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

        static internal AudioCodecInfo Create(RTCRtpCodecCapability caps)
        {
            return new AudioCodecInfo(caps);
        }

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

        /// <summary>
        /// 
        /// </summary>
        /// <param name="obj"></param>
        /// <returns></returns>
        public override bool Equals(object obj)
        {
            return obj is AudioCodecInfo ? Equals((AudioCodecInfo)obj) : base.Equals(obj);
        }

        /// <summary>
        /// 
        /// </summary>
        /// <returns></returns>
        public override int GetHashCode()
        {
            return new { mimeType, sdpFmtpLine, channelCount, sampleRate }.GetHashCode();
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="left"></param>
        /// <param name="right"></param>
        /// <returns></returns>
        public static bool operator ==(AudioCodecInfo left, AudioCodecInfo right)
        {
            if (ReferenceEquals(left, null))
            {
                return ReferenceEquals(left, null);
            }
            else
            {
                return left.Equals(right);
            }
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="left"></param>
        /// <param name="right"></param>
        /// <returns></returns>
        public static bool operator !=(AudioCodecInfo left, AudioCodecInfo right)
        {
            return !(left == right);
        }

        internal AudioCodecInfo(RTCRtpCodecCapability cap)
        {
            m_MimeType = cap.mimeType;
            m_SdpFmtpLine = cap.sdpFmtpLine;
            m_ChannelCount = cap.channels.GetValueOrDefault();
            m_SampleRate = cap.clockRate.GetValueOrDefault();
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
    }
}
