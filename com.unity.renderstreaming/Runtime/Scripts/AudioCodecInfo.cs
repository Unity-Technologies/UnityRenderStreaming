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
        private string m_codecName;
        [SerializeField]
        private string m_sdpFmtpLine;

        /// <summary>
        /// 
        /// </summary>
        public string name { get { return m_codecName; } }

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

        /// <summary>
        /// 
        /// </summary>
        public string sdpFmtpLine { get { return m_sdpFmtpLine; } }

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
            m_codecName = capability.GetCodecName();
            m_sdpFmtpLine = capability.sdpFmtpLine;
        }
    }
}
