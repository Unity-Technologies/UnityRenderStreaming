using Unity.WebRTC;

namespace Unity.RenderStreaming
{
    internal static class RTCRtpCodecCapabilityExtension
    {
        public static string GetCodecName(this RTCRtpCodecCapability cap)
        {
            return cap?.mimeType.Split('/')[1];
        }
    }
}
