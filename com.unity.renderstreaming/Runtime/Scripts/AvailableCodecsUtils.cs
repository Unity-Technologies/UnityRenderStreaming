using System.Collections.Generic;
using System.Linq;
using Unity.WebRTC;

namespace Unity.RenderStreaming
{
    public class AvailableCodecsUtils
    {
        public enum TrackType
        {
            Audio,
            Video
        }

        private static readonly string[] s_excludeCodecMimeType = {"video/red", "video/ulpfec", "video/rtx"};
        private static Dictionary<int, RTCRtpCodecCapability> s_availableCodecs;

        public static IReadOnlyDictionary<int, RTCRtpCodecCapability> AvailableCodecs
        {
            get
            {
                return s_availableCodecs;
            }
        }

        /// <summary>
        /// int: codec index
        /// string: codec name
        /// </summary>
        /// <returns></returns>
        public static IReadOnlyDictionary<int, string> GetAvailableCodecsName(TrackType kind)
        {
            if (s_availableCodecs == null)
            {
                s_availableCodecs = RTCRtpReceiver.GetCapabilities((TrackKind)kind).codecs
                    .Where(codec => !s_excludeCodecMimeType.Contains(codec.mimeType))
                    .Select((codec, index) => new {codec, index})
                    .ToDictionary(t => t.index, t => t.codec);
            }

            return s_availableCodecs.ToDictionary(pair => pair.Key, pair =>
            {
                var codec = pair.Value;
                return $"{codec.mimeType} {codec.sdpFmtpLine}";
            });
        }
    }
}
