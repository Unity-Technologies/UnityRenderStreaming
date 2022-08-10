using System;
using System.Collections.Generic;
using System.Linq;
using Unity.WebRTC;

namespace Unity.RenderStreaming
{
    public class AvailableCodecsUtils
    {
        private static readonly string[] s_excludeCodecMimeType =
        {
            "video/red", "video/ulpfec", "video/rtx", "video/flexfec-03"
        };
        private static Dictionary<int, RTCRtpCodecCapability> s_availableVideoCodecs;
        private static Dictionary<int, RTCRtpCodecCapability> s_availableAudioCodecs;

        internal static bool TryGetAvailableVideoCodec(int index, out RTCRtpCodecCapability codec)
        {
            return s_availableVideoCodecs.TryGetValue(index, out codec);
        }

        internal static bool TryGetAvailableAudioCodec(int index, out RTCRtpCodecCapability codec)
        {
            return s_availableAudioCodecs.TryGetValue(index, out codec);
        }

        /// <summary>
        /// int: codec index
        /// string: codec name
        /// </summary>
        /// <returns></returns>
        public static IReadOnlyDictionary<int, string> GetAvailableVideoCodecsName()
        {
            if (s_availableVideoCodecs == null)
            {
                s_availableVideoCodecs = RTCRtpReceiver.GetCapabilities(TrackKind.Video).codecs
                    .Where(codec => !s_excludeCodecMimeType.Contains(codec.mimeType))
                    .Select((codec, index) => new {codec, index})
                    .ToDictionary(t => t.index, t => t.codec);
            }

            return s_availableVideoCodecs.ToDictionary(pair => pair.Key, pair =>
            {
                var codec = pair.Value;
                return $"{codec.mimeType} {codec.sdpFmtpLine}";
            });
        }

        /// <summary>
        /// int: codec index
        /// string: codec name
        /// </summary>
        /// <returns></returns>
        public static IReadOnlyDictionary<int, string> GetAvailableAudioCodecsName()
        {
            if (s_availableAudioCodecs == null)
            {
                s_availableAudioCodecs = RTCRtpReceiver.GetCapabilities(TrackKind.Audio).codecs
                    .Select((codec, index) => new {codec, index})
                    .ToDictionary(t => t.index, t => t.codec);
            }

            return s_availableAudioCodecs.ToDictionary(pair => pair.Key, pair =>
            {
                var codec = pair.Value;
                return $"{codec.mimeType} {codec.sdpFmtpLine}";
            });
        }
    }
}
