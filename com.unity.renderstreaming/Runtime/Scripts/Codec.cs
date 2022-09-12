using System.Collections.Generic;
using System.Linq;
using Unity.WebRTC;

namespace Unity.RenderStreaming
{
    internal static class RTCRtpCodecCapabilityExtension
    {
        public static string GetCodecName(this string mimeType)
        {
            if (mimeType == null)
                return null;
            string[] substrings = mimeType.Split('/');
            if (substrings.Length > 1)
                return substrings[1];
            return null;
        }

        public static IEnumerable<RTCRtpCodecCapability> SelectCodecCapabilities(this RTCRtpCapabilities capabilities, IEnumerable<VideoCodecInfo> codecs)
        {
            var caps = capabilities.codecs;
            return codecs
                .Where(codec => codec != null)
                .Select(codec => caps.FirstOrDefault(cap => codec.Equals(cap)))
                .Where(cap => cap != null);
        }
        public static IEnumerable<RTCRtpCodecCapability> SelectCodecCapabilities(this RTCRtpCapabilities capabilities, IEnumerable<AudioCodecInfo> codecs)
        {
            var caps = capabilities.codecs;
            return codecs
                .Where(codec => codec != null)
                .Select(codec => caps.FirstOrDefault(cap => codec.Equals(cap)))
                .Where(cap => cap != null);
        }
    }
}
