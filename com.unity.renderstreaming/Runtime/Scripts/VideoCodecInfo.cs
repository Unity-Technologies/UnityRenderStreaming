using System;
using System.Collections.Generic;
using Unity.WebRTC;

namespace Unity.RenderStreaming
{
    /// <summary>
    /// 
    /// </summary>
    public class VideoCodecInfo : IEquatable<VideoCodecInfo>
    {
        const string KeyCodecImplementation = "implementation_name";

        public string name { get { return capability.mimeType.Split('/')[1]; } }

        /// <summary>
        /// 
        /// </summary>
        public string mimeType { get { return capability.mimeType; } }

        /// <summary>
        /// 
        /// </summary>
        public string CodecImplementation { get { return parameters[KeyCodecImplementation]; } }

        public bool Equals(VideoCodecInfo other)
        {
            if (other == null)
                return false;
            return this.capability.mimeType == other.capability.mimeType
                && this.capability.sdpFmtpLine == other.capability.sdpFmtpLine;
        }

        internal RTCRtpCodecCapability capability;

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
        protected VideoCodecInfo(RTCRtpCodecCapability caps)
        {
            capability = caps;

            string[] subs = capability.sdpFmtpLine.Split(';');

            foreach(string sub in subs)
            {
                string[] pair = sub.Split('=');
                parameters.Add(pair[0], pair[1]);
            }
        }
    }


    public enum VP9Profile
    {
        Profile0 = 0,
        Profile1 = 1,
        Profile2 = 2,
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
        public VP9Profile profile
        {
            get { return (VP9Profile)Enum.ToObject(typeof(VP9Profile), Convert.ToInt32(parameters[KeyProfileId])); }
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
        ConstrainedBaseline = 0x42e0,
        Baseline = 0x4200,
        ProfileMain = 0x4d00,
        ConstrainedHigh = 0x640c,
        High = 0x6400,
    }

    /// <summary>
    /// 
    /// </summary>
    public class H264CodecInfo : VideoCodecInfo
    {
        const string KeyProfileLevelId = "profile-level-id";
        public H264Profile profile
        {
            get { return (H264Profile)Enum.ToObject(typeof(H264Profile), Convert.ToInt32(parameters[KeyProfileLevelId], 16) >> 8); }
        }
        public int level { get { return Convert.ToInt32(parameters[KeyProfileLevelId], 16) & 0xFF; } }

        internal H264CodecInfo(RTCRtpCodecCapability caps) : base(caps)
        {
        }
    }

}
