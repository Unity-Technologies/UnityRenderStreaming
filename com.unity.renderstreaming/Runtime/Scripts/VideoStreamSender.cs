using Unity.WebRTC;
using UnityEngine;
using System;
using System.Collections.Generic;
using System.Linq;

namespace Unity.RenderStreaming
{
    internal sealed class StreamingSizeAttribute : PropertyAttribute { }

    internal sealed class FrameRateAttribute : PropertyAttribute { }

    internal sealed class BitrateAttribute : PropertyAttribute { }

    internal sealed class RenderTextureAntiAliasingAttribute : PropertyAttribute { }

    internal sealed class RenderTextureDepthBufferAttribute : PropertyAttribute { }

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

        public static RTCError SetScaleResolutionDown(this RTCRtpSender sender, double scaleFactor)
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

    internal static class RTCRtpTransceiverExtension
    {
        public static RTCErrorType SetCodec(this RTCRtpTransceiver transceiver, string mimetype)
        {
            var capabilities = RTCRtpSender.GetCapabilities(transceiver.Sender.Track.Kind);
            var codecs = capabilities.codecs.Where(codec => codec.mimeType == mimetype);
            return transceiver.SetCodecPreferences(codecs.ToArray());
        }
    }

    /// <summary>
    ///
    /// </summary>
    public class VideoStreamSender : StreamSenderBase
    {
        // todo(kazuki): check default value.
        const float s_defaultFrameRate = 30;
        // todo(kazuki): check default value.
        const uint s_defaultBitrate = 1000;

        //todo(kazuki): remove this value.
        [SerializeField, StreamingSize]
        public Vector2Int streamingSize = new Vector2Int(1280, 720);

        [SerializeField, FrameRate]
        private float m_frameRate = s_defaultFrameRate;

        [SerializeField]
        private uint m_minBitrate = s_defaultBitrate;

        [SerializeField]
        private uint m_maxBitrate = s_defaultBitrate;

        [SerializeField]
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
            get { return m_minBitrate; }
        }

        /// <summary>
        ///
        /// </summary>
        public uint maxBitrate
        {
            get { return m_maxBitrate; }
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
        /// todo(kazuki):: rename `targetTexture`
        public virtual Texture SendTexture { get; }

        /// <summary>
        ///
        /// </summary>
        /// <param name="index"></param>
        /// todo(kazuki)::rename `SetCodec`
        public void FilterCodec(string connectionId, int index)
        {
            if (!Transceivers.TryGetValue(connectionId, out var transceiver))
                return;
            RTCRtpSendParameters parameters = transceiver.Sender.GetParameters();
            var encodings = parameters.encodings.ToList().GetRange(index, 1);
            parameters.encodings = encodings.ToArray();
            transceiver.Sender.SetParameters(parameters);
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
                    Debug.LogError(error.message);
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
            m_minBitrate = minBitrate;
            m_maxBitrate = maxBitrate;
            foreach (var transceiver in Transceivers.Values)
            {
                RTCError error = transceiver.Sender.SetBitrate(m_minBitrate, m_maxBitrate);
                if (error.errorType != RTCErrorType.None)
                    Debug.LogError(error.message);
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
                RTCError error = transceiver.Sender.SetScaleResolutionDown(m_scaleFactor);
                if (error.errorType != RTCErrorType.None)
                    Debug.LogError(error.message);
            }
        }
    }
}
