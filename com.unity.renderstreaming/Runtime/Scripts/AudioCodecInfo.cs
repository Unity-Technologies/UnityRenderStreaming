using System;
using Unity.WebRTC;

namespace Unity.RenderStreaming
{
    /// <summary>
    /// 
    /// </summary>
    public class AudioCodecInfo : IEquatable<AudioCodecInfo>
    {
        /// <summary>
        /// 
        /// </summary>
        public string name { get { return capability.mimeType.Split('/')[1]; } }

        /// <summary>
        /// 
        /// </summary>
        public string mimeType { get { return capability.mimeType; } }

        /// <summary>
        /// 
        /// </summary>
        public int channelCount { get { return capability.channels.Value; } }

        /// <summary>
        /// 
        /// </summary>
        public int sampleRate { get { return capability.clockRate.Value; } }

        internal RTCRtpCodecCapability capability;

        /// <summary>
        /// 
        /// </summary>
        /// <param name="other"></param>
        /// <returns></returns>
        public bool Equals(AudioCodecInfo other)
        {
            if (other == null)
                return false;
            return this.capability.mimeType == other.capability.mimeType
                && this.capability.sdpFmtpLine == other.capability.sdpFmtpLine;
        }

        internal AudioCodecInfo(RTCRtpCodecCapability caps)
        {
            capability = caps;
        }
    }
}
