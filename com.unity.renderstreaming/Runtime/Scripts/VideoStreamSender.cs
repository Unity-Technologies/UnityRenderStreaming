using Unity.WebRTC;
using UnityEngine;
using System;
using System.Collections.Generic;
using System.Linq;

namespace Unity.RenderStreaming
{
    internal sealed class StreamingSizeAttribute : PropertyAttribute { }

    internal sealed class FrameRateAttribute : PropertyAttribute { }

    internal sealed class BitrateAttribute : PropertyAttribute
    {
        public int minValue;
        public int maxValue;

        public BitrateAttribute(int minValue, int maxValue)
        {
            this.minValue = minValue;
            this.maxValue = maxValue;
        }
    }

    internal sealed class RenderTextureAntiAliasingAttribute : PropertyAttribute { }

    internal sealed class RenderTextureDepthBufferAttribute : PropertyAttribute { }

    internal sealed class WebCamDeviceAttribute : PropertyAttribute { }

    internal sealed class ScaleResolutionAttribute : PropertyAttribute { }

    [Serializable]
    internal struct Range
    {
        public uint min;
        public uint max;

        public Range(uint min, uint max) { this.min = min; this.max = max; }
    }

    internal sealed class CodecAttribute : PropertyAttribute { }

    internal static class RTCRtpSenderExtension
    {
        public static RTCError SetFrameRate(this RTCRtpSender sender, uint framerate)
        {
            if (sender.Track.Kind != TrackKind.Video)
                throw new ArgumentException();

            RTCRtpSendParameters parameters = sender.GetParameters();
            foreach (var encoding in parameters.encodings)
            {
                encoding.maxFramerate = framerate;
            }
            return sender.SetParameters(parameters);
        }

        public static RTCError SetScaleResolutionDown(this RTCRtpSender sender, double? scaleFactor)
        {
            if (sender.Track.Kind != TrackKind.Video)
                throw new ArgumentException();

            RTCRtpSendParameters parameters = sender.GetParameters();
            foreach (var encoding in parameters.encodings)
            {
                encoding.scaleResolutionDownBy = scaleFactor;
            }
            return sender.SetParameters(parameters);
        }

        public static RTCError SetBitrate(this RTCRtpSender sender, uint? minBitrate, uint? maxBitrate)
        {
            RTCRtpSendParameters parameters = sender.GetParameters();

            foreach (var encoding in parameters.encodings)
            {
                encoding.minBitrate = minBitrate * 1000;
                encoding.maxBitrate = maxBitrate * 1000;
            }
            return sender.SetParameters(parameters);
        }
    }

    /// <summary>
    ///
    /// </summary>
    public class VideoStreamSender : StreamSenderBase
    {
        // todo(kazuki): check default value.
        const float s_defaultFrameRate = 30;

        const uint s_defaultMinBitrate = 0;
        const uint s_defaultMaxBitrate = 1000;

        //todo(kazuki): remove this value.
        [SerializeField, StreamingSize]
        public Vector2Int streamingSize = new Vector2Int(1280, 720);

        [SerializeField, Codec]
        private Codec<VideoStreamSender> m_codec;

        [SerializeField, FrameRate]
        private float m_frameRate = s_defaultFrameRate;

        [SerializeField, Bitrate(0, 10000)]
        private Range m_bitrate = new Range(s_defaultMinBitrate, s_defaultMaxBitrate);

        [SerializeField, ScaleResolution]
        private float m_scaleFactor = 1f;

        /// <summary>
        ///
        /// </summary>
        public float frameRate
        {
            get { return m_frameRate; }
        }

        /// <summary>
        ///
        /// </summary>
        public uint minBitrate
        {
            get { return m_bitrate.min; }
        }

        /// <summary>
        ///
        /// </summary>
        public uint maxBitrate
        {
            get { return m_bitrate.max; }
        }

        /// <summary>
        /// 
        /// </summary>
        public float scaleResolutionDown
        {
            get { return m_scaleFactor; }
        }

        /// <summary>
        /// 
        /// </summary>
        public VideoCodecInfo codec
        {
            get { return (VideoCodecInfo)m_codec; }
        }

        /// <summary>
        ///
        /// </summary>
        /// todo(kazuki):: rename `targetTexture`
        public virtual Texture SendTexture { get; }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="mimeType"></param>
        public void SetCodec(VideoCodecInfo codec)
        {
            m_codec = (Codec<VideoStreamSender>)codec;
            foreach (var transceiver in Transceivers.Values)
            {
                if(!string.IsNullOrEmpty(transceiver.Mid))
                    continue;
                if (transceiver.Sender.Track.ReadyState == TrackState.Ended)
                    continue;

                RTCErrorType error = transceiver.SetCodec(new VideoCodecInfo[] { (VideoCodecInfo)m_codec });
                if (error != RTCErrorType.None)
                    throw new InvalidOperationException($"Set codec is failed. errorCode={error}");
            }
        }

        /// <summary>
        /// 
        /// </summary>
        /// <returns></returns>
        public static IEnumerable<VideoCodecInfo> GetAvailableCodecs()
        {
            string[] excludeCodecMimeType = { "video/red", "video/ulpfec", "video/rtx", "video/flexfec-03" };
            var capabilities = RTCRtpSender.GetCapabilities(TrackKind.Video);
            return capabilities.codecs.Where(codec => !excludeCodecMimeType.Contains(codec.mimeType)).Select(codec => VideoCodecInfo.Create(codec));
        }

        /// <summary>
        ///
        /// </summary>
        /// <param name="frameRate"></param>
        public void SetFrameRate(float frameRate)
        {
            if (frameRate < 0)
                throw new ArgumentOutOfRangeException("frameRate", frameRate, "The parameter must be greater than zero.");
            m_frameRate = frameRate;
            foreach (var transceiver in Transceivers.Values)
            {
                RTCError error = transceiver.Sender.SetFrameRate((uint)m_frameRate);
                if (error.errorType != RTCErrorType.None)
                    throw new InvalidOperationException($"Set framerate is failed. {error.message}");
            }
        }

        /// <summary>
        ///
        /// </summary>
        /// <param name="bitrate"></param>
        public void SetBitrate(uint minBitrate, uint maxBitrate)
        {
            if (minBitrate > maxBitrate)
                throw new ArgumentException("The maxBitrate must be greater than minBitrate.", "maxBitrate");
            m_bitrate.min = minBitrate;
            m_bitrate.max = maxBitrate;
            foreach (var transceiver in Transceivers.Values)
            {
                RTCError error = transceiver.Sender.SetBitrate(m_bitrate.min, m_bitrate.max);
                if (error.errorType != RTCErrorType.None)
                    throw new InvalidOperationException($"Set codec is failed. {error.message}");
            }
        }

        /// <summary>
        ///
        /// </summary>
        /// <param name="scaleFactor">The parameter must be greater than 1.0f.</param>
        public void SetScaleResolutionDown(float scaleFactor)
        {
            if (scaleFactor < 1.0f)
                throw new ArgumentOutOfRangeException("scaleFactor", scaleFactor, "The parameter must be greater than 1.0f. Scaleup is not allowed.");

            m_scaleFactor = scaleFactor;
            foreach (var transceiver in Transceivers.Values)
            {
                double? value = Mathf.Approximately(m_scaleFactor, 1) ? (double?)null : m_scaleFactor;
                RTCError error = transceiver.Sender.SetScaleResolutionDown(value);
                if (error.errorType != RTCErrorType.None)
                    throw new InvalidOperationException($"Set codec is failed. {error.message}");
            }
        }
    }
}
