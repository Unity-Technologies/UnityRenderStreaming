using System;
using Unity.WebRTC;
using UnityEngine;

namespace Unity.RenderStreaming
{
    /// <summary>
    /// Represents information about an audio codec, including its MIME type, SDP format parameters, channel count, and sample rate.
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
        /// Gets the name of the audio codec.
        /// </summary>
        public string name { get { return m_MimeType.GetCodecName(); } }

        /// <summary>
        /// Gets the MIME type of the audio codec.
        /// </summary>
        public string mimeType { get { return m_MimeType; } }

        /// <summary>
        /// Gets the number of audio channels.
        /// </summary>
        public int channelCount { get { return m_ChannelCount; } }

        /// <summary>
        /// Gets the sample rate of the audio.
        /// </summary>
        public int sampleRate { get { return m_SampleRate; } }

        /// <summary>
        /// Gets the SDP format parameters line.
        /// </summary>
        public string sdpFmtpLine { get { return m_SdpFmtpLine; } }

        static internal AudioCodecInfo Create(RTCRtpCodecCapability caps)
        {
            return new AudioCodecInfo(caps);
        }

        /// <summary>
        /// Determines whether the specified <see cref="AudioCodecInfo"/> is equal to the current <see cref="AudioCodecInfo"/>.
        /// </summary>
        /// <example>
        /// <code>
        /// <![CDATA[
        /// Debug.Log(audioStreamReceiver1.codec.Equals(audioStreamReceiver2.codec));
        /// ]]>
        ///</code>
        /// </example>
        /// <param name="other">The <see cref="AudioCodecInfo"/> to compare with the current <see cref="AudioCodecInfo"/>.</param>
        /// <returns>true if the specified <see cref="AudioCodecInfo"/> is equal to the current <see cref="AudioCodecInfo"/>; otherwise, false.</returns>
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
        /// Determines whether the specified object is equal to the current <see cref="AudioCodecInfo"/>.
        /// </summary>
        /// <param name="obj">The object to compare with the current <see cref="AudioCodecInfo"/>.</param>
        /// <returns>true if the specified object is equal to the current <see cref="AudioCodecInfo"/>; otherwise, false.</returns>
        public override bool Equals(object obj)
        {
            return obj is AudioCodecInfo ? Equals((AudioCodecInfo)obj) : base.Equals(obj);
        }

        /// <summary>
        /// Returns a hash code for the <see cref="AudioCodecInfo"/>.
        /// </summary>
        /// <returns>A hash code for the current <see cref="AudioCodecInfo"/>.</returns>
        public override int GetHashCode()
        {
            return new { mimeType, sdpFmtpLine, channelCount, sampleRate }.GetHashCode();
        }

        /// <summary>
        /// Determines whether two specified instances of <see cref="AudioCodecInfo"/> are equal.
        /// </summary>
        /// <param name="left">The first <see cref="AudioCodecInfo"/> to compare.</param>
        /// <param name="right">The second <see cref="AudioCodecInfo"/> to compare.</param>
        /// <returns>true if the two <see cref="AudioCodecInfo"/> instances are equal; otherwise, false.</returns>
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
        /// Determines whether two specified instances of <see cref="AudioCodecInfo"/> are not equal.
        /// </summary>
        /// <param name="left">The first <see cref="AudioCodecInfo"/> to compare.</param>
        /// <param name="right">The second <see cref="AudioCodecInfo"/> to compare.</param>
        /// <returns>true if the two <see cref="AudioCodecInfo"/> instances are not equal; otherwise, false.</returns>
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
