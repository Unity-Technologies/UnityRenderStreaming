using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using Unity.WebRTC;
using UnityEngine;

namespace Unity.RenderStreaming
{
    /// <summary>
    /// Enum representing the video render mode.
    /// </summary>
    public enum VideoRenderMode
    {
        /// <summary>
        /// Render to a RenderTexture.
        /// </summary>
        RenderTexture,
        /// <summary>
        /// API only, no rendering.
        /// </summary>
        APIOnly,
    }

    /// <summary>
    /// Component for receiving video streams.
    /// </summary>
    [AddComponentMenu("Render Streaming/Video Stream Receiver")]
    public class VideoStreamReceiver : StreamReceiverBase
    {
        internal const string CodecPropertyName = nameof(m_Codec);
        internal const string RenderModePropertyName = nameof(m_RenderMode);
        internal const string TargetTexturePropertyName = nameof(m_TargetTexture);

        /// <summary>
        /// Delegate for updating the received texture.
        /// </summary>
        /// <param name="receiveTexture">The received texture.</param>
        public delegate void OnUpdateReceiveTextureHandler(Texture receiveTexture);

        /// <summary>
        /// Event triggered when the received texture is updated.
        /// </summary>
        public OnUpdateReceiveTextureHandler OnUpdateReceiveTexture;

        [SerializeField, Codec]
        private VideoCodecInfo m_Codec;

        [SerializeField]
        private VideoRenderMode m_RenderMode;

        [SerializeField]
        private RenderTexture m_TargetTexture;

        /// <summary>
        /// Gets the codec information for the video stream.
        /// </summary>
        public VideoCodecInfo codec
        {
            get { return m_Codec; }
        }

        /// <summary>
        /// The width of the received video stream.
        /// </summary>
        public int width => m_texture.width;

        /// <summary>
        /// The height of the received video stream.
        /// </summary>
        public int height => m_texture.height;

        /// <summary>
        /// The texture of the received video stream.
        /// </summary>
        public Texture texture => m_texture;

        /// <summary>
        /// The target RenderTexture.
        /// </summary>
        public RenderTexture targetTexture
        {
            get { return m_TargetTexture; }
            set { m_TargetTexture = value; }
        }

        private Texture m_texture;

        private Coroutine m_coroutine;


        /// <summary>
        /// Gets the available video codecs.
        /// </summary>
        /// <code>
        /// var codecs = VideoStreamSender.GetAvailableCodecs();
        /// foreach (var codec in codecs)
        ///     Debug.Log(codec.name);
        /// </code>
        /// </example>
        /// <returns>A list of available codecs.</returns>
        public static IEnumerable<VideoCodecInfo> GetAvailableCodecs()
        {
            string[] excludeCodecMimeType = { "video/red", "video/ulpfec", "video/rtx", "video/flexfec-03" };
            var capabilities = RTCRtpReceiver.GetCapabilities(TrackKind.Video);
            return capabilities.codecs.Where(codec => !excludeCodecMimeType.Contains(codec.mimeType)).Select(codec => VideoCodecInfo.Create(codec));
        }

        /// <summary>
        /// Sets the codec for the video stream.
        /// </summary>
        /// <example>
        /// <code>
        /// <![CDATA[
        /// var codec = VideoStreamReceiver.GetAvailableCodecs().FirstOrDefault(x => x.mimeType.Contains("VP9"));
        /// videoStreamReceiver.SetCodec(codec);
        /// ]]>
        ///</code>
        /// </example>
        /// <param name="codec">The codec information to set.</param>
        /// <exception cref="InvalidOperationException">Thrown if the transceiver is streaming or the track has ended.</exception>
        public void SetCodec(VideoCodecInfo codec)
        {
            m_Codec = codec;
            if (Transceiver == null)
                return;
            if (!string.IsNullOrEmpty(Transceiver.Mid))
                throw new InvalidOperationException("Transceiver is streaming. This operation is invalid during the track is in use.");
            if (Transceiver.Sender.Track.ReadyState == TrackState.Ended)
                throw new InvalidOperationException("Track has already been ended.");

            var codecs = new VideoCodecInfo[] { m_Codec };
            RTCErrorType error = Transceiver.SetCodecPreferences(SelectCodecCapabilities(codecs).ToArray());
            if (error != RTCErrorType.None)
                throw new InvalidOperationException($"Set codec is failed. errorCode={error}");
        }

        internal IEnumerable<RTCRtpCodecCapability> SelectCodecCapabilities(IEnumerable<VideoCodecInfo> codecs)
        {
            return RTCRtpReceiver.GetCapabilities(TrackKind.Video).SelectCodecCapabilities(codecs);
        }

        private protected virtual void Awake()
        {
            OnStartedStream += StartedStream;
            OnStoppedStream += StoppedStream;
        }

        private void StartedStream(string connectionId)
        {
            if (Track is VideoStreamTrack videoTrack)
            {
                videoTrack.OnVideoReceived += texture =>
                {
                    m_texture = texture;
                    OnUpdateReceiveTexture?.Invoke(m_texture);
                };
            }
            m_coroutine = StartCoroutine(Render());
        }

        private void StoppedStream(string connectionId)
        {
            m_texture = null;
            OnUpdateReceiveTexture?.Invoke(m_texture);
            if (m_coroutine != null)
            {
                StopCoroutine(m_coroutine);
                m_coroutine = null;
            }
        }

        private IEnumerator Render()
        {
            while (true)
            {
                yield return new WaitForEndOfFrame();
                if (m_RenderMode != VideoRenderMode.RenderTexture ||
                    m_texture == null ||
                    m_TargetTexture == null)
                    continue;
                Graphics.Blit(m_texture, m_TargetTexture);
            }
        }
    }
}
