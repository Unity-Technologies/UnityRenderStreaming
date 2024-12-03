using System;
using System.Collections.Generic;
using Unity.WebRTC;
using UnityEngine;

namespace Unity.RenderStreaming
{
    /// <summary>
    /// Represents information about a video codec, including its MIME type, SDP format parameters.
    /// </summary>
    [Serializable]
    public class VideoCodecInfo : IEquatable<VideoCodecInfo>
    {
        static readonly string KeyCodecImplementation = "implementation_name";

        [SerializeField]
        private string m_MimeType;
        [SerializeField]
        private string m_SdpFmtpLine;

        readonly Dictionary<string, string> m_parameters = new Dictionary<string, string>();

        /// <summary>
        /// Gets the name of the video codec.
        /// </summary>
        public string name { get { return m_MimeType.GetCodecName(); } }

        /// <summary>
        /// Gets the MIME type of the video codec.
        /// </summary>
        public string mimeType { get { return m_MimeType; } }

        /// <summary>
        /// Gets the codec implementation name.
        /// </summary>
        public string codecImplementation { get { return parameters[KeyCodecImplementation]; } }

        /// <summary>
        /// Gets the SDP format parameters line.
        /// </summary>
        public string sdpFmtpLine { get { return m_SdpFmtpLine; } }

        /// <summary>
        /// Determines whether the specified <see cref="VideoCodecInfo"/> is equal to the current <see cref="VideoCodecInfo"/>.
        /// </summary>
        /// <example>
        /// <code>
        /// <![CDATA[
        /// Debug.Log(videoStreamReceiver1.videoCodec.Equals(videoStreamReceiver2.videoCodec));
        /// ]]>
        ///</code>
        /// </example>
        /// <param name="other">The <see cref="VideoCodecInfo"/> to compare with the current <see cref="VideoCodecInfo"/>.</param>
        /// <returns>True if the specified <see cref="VideoCodecInfo"/> is equal to the current <see cref="VideoCodecInfo"/>; otherwise, false.</returns>
        public bool Equals(VideoCodecInfo other)
        {
            if (other == null)
                return false;
            return this.mimeType == other.mimeType
                && this.sdpFmtpLine == other.sdpFmtpLine;
        }

        /// <summary>
        /// Determines whether the specified object is equal to the current <see cref="VideoCodecInfo"/>.
        /// </summary>
        /// <param name="obj">The object to compare with the current <see cref="VideoCodecInfo"/>.</param>
        /// <returns>True if the specified object is equal to the current <see cref="VideoCodecInfo"/>; otherwise, false.</returns>
        public override bool Equals(object obj)
        {
            return obj is VideoCodecInfo ? Equals((VideoCodecInfo)obj) : base.Equals(obj);
        }

        /// <summary>
        /// Returns a hash code for the <see cref="VideoCodecInfo"/>.
        /// The hash code is based on the MIME type and SDP format parameters line properties.
        /// </summary>
        /// <returns>A hash code for the current <see cref="VideoCodecInfo"/>.</returns>
        public override int GetHashCode()
        {
            return new { mimeType, sdpFmtpLine }.GetHashCode();
        }

        /// <summary>
        /// Determines whether two specified instances of <see cref="VideoCodecInfo"/> are equal.
        /// </summary>
        /// <param name="left">The first <see cref="VideoCodecInfo"/> to compare.</param>
        /// <param name="right">The second <see cref="VideoCodecInfo"/> to compare.</param>
        /// <returns>True if the two <see cref="VideoCodecInfo"/> instances are equal; otherwise, false.</returns>
        public static bool operator ==(VideoCodecInfo left, VideoCodecInfo right)
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
        /// Determines whether two specified instances of <see cref="VideoCodecInfo"/> are not equal.
        /// </summary>
        /// <param name="left">The first <see cref="VideoCodecInfo"/> to compare.</param>
        /// <param name="right">The second <see cref="VideoCodecInfo"/> to compare.</param>
        /// <returns>True if the two <see cref="VideoCodecInfo"/> instances are not equal; otherwise, false.</returns>
        public static bool operator !=(VideoCodecInfo left, VideoCodecInfo right)
        {
            return !(left == right);
        }

        protected Dictionary<string, string> parameters
        {
            get
            {
                if (m_parameters != null)
                    return m_parameters;

                if (string.IsNullOrEmpty(m_SdpFmtpLine))
                    return null;
                string[] subs = m_SdpFmtpLine.Split(';');
                foreach (string sub in subs)
                {
                    string[] pair = sub.Split('=');
                    m_parameters.Add(pair[0], pair[1]);
                }
                return m_parameters;
            }
        }

        static internal VideoCodecInfo Create(RTCRtpCodecCapability caps)
        {
            switch (caps.mimeType)
            {
                case "video/H264":
                    return new H264CodecInfo(caps);
                case "video/VP9":
                    return new VP9CodecInfo(caps);
                case "video/AV1":
                    return new AV1CodecInfo(caps);
                default:
                    return new VideoCodecInfo(caps);
            }
        }

        internal bool Equals(RTCRtpCodecCapability other)
        {
            if (other == null)
                return false;
            return this.mimeType == other.mimeType
                && this.sdpFmtpLine == other.sdpFmtpLine;
        }

        internal VideoCodecInfo(RTCRtpCodecCapability caps)
        {
            m_MimeType = caps.mimeType;
            m_SdpFmtpLine = caps.sdpFmtpLine;

            string[] subs = m_SdpFmtpLine.Split(';');
            foreach (string sub in subs)
            {
                string[] pair = sub.Split('=');
                parameters.Add(pair[0], pair[1]);
            }
        }
    }

    /// <summary>
    /// Represents the profiles for the VP9 video codec.
    /// </summary>
    public enum VP9Profile
    {
        /// <summary>
        /// Profile 0.
        /// </summary>
        Profile0 = 0,
        /// <summary>
        /// Profile 1.
        /// </summary>
        Profile1 = 1,
        /// <summary>
        /// Profile 2.
        /// </summary>
        Profile2 = 2,
        /// <summary>
        /// Profile 3.
        /// </summary>
        Profile3 = 3,
    }

    /// <summary>
    /// Represents information about the VP9 video codec.
    /// </summary>
    public class VP9CodecInfo : VideoCodecInfo
    {
        const string KeyProfileId = "profile-id";

        /// <summary>
        /// Gets the profile of the VP9 video codec.
        /// </summary>
        public VP9Profile? profile
        {
            get
            {
                if (parameters.TryGetValue(KeyProfileId, out var value))
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
    /// Represents the profiles for the H264 video codec.
    /// </summary>
    public enum H264Profile
    {
        /// <summary>
        /// Constrained Baseline Profile.
        /// </summary>
        ConstrainedBaseline = 0x42e0,
        /// <summary>
        /// Baseline Profile.
        /// </summary>
        Baseline = 0x4200,
        /// <summary>
        /// Main Profile.
        /// </summary>
        Main = 0x4d00,
        /// <summary>
        /// Constrained High Profile.
        /// </summary>
        ConstrainedHigh = 0x640c,
        /// <summary>
        /// High Profile.
        /// </summary>
        High = 0x6400,
    }

    /// <summary>
    /// Represents information about the H264 video codec.
    /// </summary>
    public class H264CodecInfo : VideoCodecInfo
    {
        const string KeyProfileLevelId = "profile-level-id";

        /// <summary>
        /// Gets the profile of the H264 video codec.
        /// </summary>
        public H264Profile profile
        {
            get { return (H264Profile)Enum.ToObject(typeof(H264Profile), Convert.ToInt32(parameters[KeyProfileLevelId], 16) >> 8); }
        }

        /// <summary>
        /// Gets the level of the H264 video codec.
        /// </summary>
        public int level { get { return Convert.ToInt32(parameters[KeyProfileLevelId], 16) & 0xFF; } }

        internal H264CodecInfo(RTCRtpCodecCapability caps) : base(caps)
        {
        }
    }

    /// <summary>
    /// Represents the profiles for the AV1 video codec.
    /// </summary>
    public enum AV1Profile
    {
        /// <summary>
        /// Profile 0.
        /// </summary>
        Profile0 = 0,
        /// <summary>
        /// Profile 1.
        /// </summary>
        Profile1 = 1,
        /// <summary>
        /// Profile 2.
        /// </summary>
        Profile2 = 2,
    }

    /// <summary>
    /// Represents information about the AV1 video codec.
    /// </summary>
    public class AV1CodecInfo : VideoCodecInfo
    {
        const string KeyProfile = "profile";

        /// <summary>
        /// Gets the profile of the AV1 video codec.
        /// </summary>
        public AV1Profile profile
        {
            get
            {

                if (parameters.TryGetValue(KeyProfile, out var value))
                {
                    return (AV1Profile)Enum.ToObject(typeof(AV1Profile), Convert.ToInt32(value));
                }
                // If the parameter is not present, it MUST be inferred to be 0 (“Main” profile).
                // https://aomediacodec.github.io/av1-rtp-spec/#72-sdp-parameters
                return AV1Profile.Profile0;
            }
        }

        internal AV1CodecInfo(RTCRtpCodecCapability caps) : base(caps)
        {
        }
    }
}
