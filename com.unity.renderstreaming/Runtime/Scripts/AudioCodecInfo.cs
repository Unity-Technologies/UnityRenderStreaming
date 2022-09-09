using System;
using System.Collections.Generic;
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

        /// <summary>
        /// 
        /// </summary>
        /// <param name="other"></param>
        /// <returns></returns>
        public bool Equals(AudioCodecInfo other)
        {
            return this.mimeType == other.mimeType
                && this.sdpFmtpLine == other.sdpFmtpLine
                && this.channelCount == other.channelCount
                && this.sampleRate == other.sampleRate;
        }

        public override bool Equals(object obj)
        {
            return obj is AudioCodecInfo ? Equals((AudioCodecInfo)obj) : base.Equals(obj);
        }

        public override int GetHashCode()
        {
            return HashCode.Combine(mimeType, sdpFmtpLine, channelCount, sampleRate);
        }

        public static bool operator ==(AudioCodecInfo left, AudioCodecInfo right)
        {
            return left.Equals(right);
        }
        
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
    }
}
