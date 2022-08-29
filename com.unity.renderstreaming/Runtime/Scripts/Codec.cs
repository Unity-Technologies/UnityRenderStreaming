using System;
using System.Collections.Generic;
using Unity.WebRTC;

namespace Unity.RenderStreaming
{
    [Serializable]
    internal struct Codec<T> : IEquatable<RTCRtpCodecCapability>
    {
        public string mimeType;
        public string sdpFmtpLine;

        public static IEnumerable<VideoCodecInfo> GetAvailableVideoCodecs(Type type)
        {
            if (type == typeof(VideoStreamSender))
            {
                return VideoStreamSender.GetAvailableCodecs();
            }
            if (type == typeof(VideoStreamReceiver))
            {
                return VideoStreamReceiver.GetAvailableCodecs();
            }
            return null;
        }

        public static IEnumerable<AudioCodecInfo> GetAvailableAudioCodecs(Type type)
        {
            if (type == typeof(AudioStreamSender))
            {
                return AudioStreamSender.GetAvailableCodecs();
            }
            if (type == typeof(AudioStreamReceiver))
            {
                return AudioStreamReceiver.GetAvailableCodecs();
            }
            return null;
        }

        public static explicit operator VideoCodecInfo(Codec<T> codec)
        {
            foreach (var codec_ in GetAvailableVideoCodecs(typeof(T)))
            {
                if (codec.Equals(codec_.capability))
                {
                    return codec_;
                }
            }
            return null;
        }

        public static explicit operator AudioCodecInfo(Codec<T> codec)
        {
            foreach (var codec_ in GetAvailableAudioCodecs(typeof(T)))
            {
                if (codec.Equals(codec_.capability))
                {
                    return codec_;
                }
            }
            return null;
        }

        public static explicit operator Codec<T>(VideoCodecInfo codec)
        {
            return (Codec<T>)codec.capability;
        }

        public static explicit operator Codec<T>(AudioCodecInfo codec)
        {
            return (Codec<T>)codec.capability;
        }

        public static explicit operator Codec<T>(RTCRtpCodecCapability capability)
        {
            return new Codec<T>()
            {
                mimeType = capability.mimeType,
                sdpFmtpLine = capability.sdpFmtpLine
            };
        }

        public bool Equals(RTCRtpCodecCapability obj)
        {
            if (obj == null)
            {
                return false;
            }
            if (obj.mimeType == mimeType && obj.sdpFmtpLine == sdpFmtpLine)
                return true;
            return false;
        }
    }
}
