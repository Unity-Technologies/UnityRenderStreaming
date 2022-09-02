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
    public class VideoCodecInfo : IEquatable<VideoCodecInfo>
    {
        static readonly string KeyCodecImplementation = "implementation_name";

        [SerializeField]
        private string m_MimeType;
        [SerializeField]
        private string m_SdpFmtpLine;

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
        public string codecImplementation { get { return parameters[KeyCodecImplementation]; } }

        /// <summary>
        /// 
        /// </summary>
        public string sdpFmtpLine { get { return m_SdpFmtpLine; } }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="other"></param>
        /// <returns></returns>
        public bool Equals(VideoCodecInfo other)
        {
            if (other == null)
                return false;
            return this.mimeType == other.mimeType
                && this.sdpFmtpLine == other.sdpFmtpLine;
        }

        internal bool Equals(RTCRtpCodecCapability other)
        {
            if (other == null)
                return false;
            return this.mimeType == other.mimeType
                && this.sdpFmtpLine == other.sdpFmtpLine;
        }

        protected readonly Dictionary<string, string> parameters = new Dictionary<string, string>();


        static public VideoCodecInfo Create(RTCRtpCodecCapability caps)
        {
            switch(caps.mimeType)
            {
                case "video/H264":
                    return new H264CodecInfo(caps);
                case "video/VP9":
                    return new VP9CodecInfo(caps);
                default:
                    return new VideoCodecInfo(caps);
            }
        }

        protected VideoCodecInfo()
        {
            if (string.IsNullOrEmpty(m_SdpFmtpLine))
                return;
            string[] subs = m_SdpFmtpLine.Split(';');
            foreach (string sub in subs)
            {
                string[] pair = sub.Split('=');
                parameters.Add(pair[0], pair[1]);
            }
        }

        protected VideoCodecInfo(RTCRtpCodecCapability caps)
        {
            m_MimeType = caps.mimeType;
            m_SdpFmtpLine = caps.sdpFmtpLine;

            string[] subs = m_SdpFmtpLine.Split(';');
            foreach(string sub in subs)
            {
                string[] pair = sub.Split('=');
                parameters.Add(pair[0], pair[1]);
            }
        }
    }

    /// <summary>
    /// 
    /// </summary>
    public enum VP9Profile
    {
        /// <summary>
        /// 
        /// </summary>
        Profile0 = 0,
        /// <summary>
        /// 
        /// </summary>
        Profile1 = 1,
        /// <summary>
        /// 
        /// </summary>
        Profile2 = 2,
        /// <summary>
        /// 
        /// </summary>
        Profile3 = 3,
    }

    /// <summary>
    /// 
    /// </summary>
    public class VP9CodecInfo : VideoCodecInfo
    {
        const string KeyProfileId = "profile-id";

        /// <summary>
        /// 
        /// </summary>
        public VP9Profile? profile
        {
            get
            {
                if(parameters.TryGetValue(KeyProfileId, out var value))
                {
                    return (VP9Profile)Enum.ToObject(typeof(VP9Profile), Convert.ToInt32(value));
                }
                return null;
            }
        }

        internal VP9CodecInfo(RTCRtpCodecCapability caps) : base(caps)
        {
        }
    }

    /// <summary>
    /// 
    /// </summary>
    public enum H264Profile
    {
        /// <summary>
        /// 
        /// </summary>
        ConstrainedBaseline = 0x42e0,
        /// <summary>
        /// 
        /// </summary>
        Baseline = 0x4200,
        /// <summary>
        /// 
        /// </summary>
        ProfileMain = 0x4d00,
        /// <summary>
        /// 
        /// </summary>
        ConstrainedHigh = 0x640c,
        /// <summary>
        /// 
        /// </summary>
        High = 0x6400,
    }

    /// <summary>
    /// 
    /// </summary>
    public class H264CodecInfo : VideoCodecInfo
    {
        const string KeyProfileLevelId = "profile-level-id";

        /// <summary>
        /// 
        /// </summary>
        public H264Profile profile
        {
            get { return (H264Profile)Enum.ToObject(typeof(H264Profile), Convert.ToInt32(parameters[KeyProfileLevelId], 16) >> 8); }
        }

        /// <summary>
        /// 
        /// </summary>
        public int level { get { return Convert.ToInt32(parameters[KeyProfileLevelId], 16) & 0xFF; } }

        internal H264CodecInfo(RTCRtpCodecCapability caps) : base(caps)
        {
        }
    }

}
