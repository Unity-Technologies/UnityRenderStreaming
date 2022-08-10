using Unity.WebRTC;
using UnityEngine;
using System;
using System.Linq;

namespace Unity.RenderStreaming
{
    /// <summary>
    ///
    /// </summary>
    public sealed class StreamingSizeAttribute : PropertyAttribute { }

    /// <summary>
    ///
    /// </summary>
    public sealed class FramerateAttribute : PropertyAttribute { }

    /// <summary>
    ///
    /// </summary>
    public sealed class BitrateAttribute : PropertyAttribute { }

    /// <summary>
    ///
    /// </summary>
    public sealed class RenderTextureAntiAliasingAttribute : PropertyAttribute { }

    /// <summary>
    ///
    /// </summary>
    public sealed class RenderTextureDepthBufferAttribute : PropertyAttribute { }

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

        public static RTCError SetBitrate(this RTCRtpSender sender, uint? bitrate)
        {
            RTCRtpSendParameters parameters = sender.GetParameters();

            if (bitrate == null || bitrate == 0)
            {
                foreach (var encoding in parameters.encodings)
                {
                    encoding.maxBitrate = null;
                    encoding.minBitrate = null;
                }
            }
            else
            {
                foreach (var encoding in parameters.encodings)
                {
                    encoding.maxBitrate = bitrate * 1000;
                    encoding.minBitrate = bitrate * 1000;
                }
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

        [SerializeField]
        private float m_frameRate = s_defaultFrameRate;

        [SerializeField]
        private uint m_bitrate = s_defaultBitrate;

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
        public uint bitrate
        {
            get { return m_bitrate; }
        }

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
        /// <param name="frameRate"></param>
        public void SetFrameRate(float frameRate)
        {
            if (frameRate < 0)
                throw new ArgumentOutOfRangeException("framerate", frameRate, "The parameter must be greater than zero.");
            m_frameRate = frameRate;
            foreach (var transceiver in Transceivers.Values)
            {
                RTCError error = transceiver.Sender.SetFrameRate((uint)m_frameRate);
                if (error.errorType == RTCErrorType.None)
                    Debug.LogError(error.message);
            }
        }

        /// <summary>
        ///
        /// </summary>
        /// <param name="bitrate"></param>
        public void SetBitrate(uint bitrate)
        {
            if (bitrate < 0)
                throw new ArgumentOutOfRangeException("bitrate", frameRate, "The parameter must be greater than zero.");
            m_bitrate = bitrate;
            foreach (var transceiver in Transceivers.Values)
            {
                RTCError error = transceiver.Sender.SetBitrate(m_bitrate);
                if (error.errorType == RTCErrorType.None)
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
                if (error.errorType == RTCErrorType.None)
                    Debug.LogError(error.message);
            }
        }
    }
}
