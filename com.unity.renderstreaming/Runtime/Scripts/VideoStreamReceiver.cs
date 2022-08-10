using Unity.WebRTC;
using UnityEngine;
using System;
using System.Linq;
using System.Collections.Generic;

namespace Unity.RenderStreaming
{
    /// <summary>
    ///
    /// </summary>
    public class VideoStreamReceiver : StreamReceiverBase
    {
        /// <summary>
        ///
        /// </summary>
        /// <param name="receiveTexture"></param>
        public delegate void OnUpdateReceiveTextureHandler(Texture receiveTexture);

        /// <summary>
        ///
        /// </summary>
        public OnUpdateReceiveTextureHandler OnUpdateReceiveTexture;

        /// <summary>
        ///
        /// </summary>
        public override TrackKind Kind { get { return TrackKind.Video; } }

        /// <summary>
        ///
        /// </summary>
        [SerializeField] private Vector2Int streamingSize = new Vector2Int(1280, 720);


        private VideoCodecInfo m_codec;

        /// <summary>
        /// 
        /// </summary>
        public VideoCodecInfo codec
        {
            get { return m_codec; }
        }

        /// <summary>
        ///
        /// </summary>
        public Texture ReceiveTexture => m_receiveTexture;

        /// <summary>
        /// 
        /// </summary>
        /// <returns></returns>
        public static IEnumerable<VideoCodecInfo> GetAvailableCodecs()
        {
            string[] excludeCodecMimeType = { "video/red", "video/ulpfec", "video/rtx", "video/flexfec-03" };
            var capabilities = RTCRtpReceiver.GetCapabilities(TrackKind.Video);
            return capabilities.codecs.Where(codec => !excludeCodecMimeType.Contains(codec.mimeType)).Select(codec => VideoCodecInfo.Create(codec));
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="mimeType"></param>
        public void SetCodec(VideoCodecInfo codec)
        {
            m_codec = codec;

            if (Transceiver == null)
                return;
            if (!string.IsNullOrEmpty(Transceiver.Mid))
                throw new InvalidOperationException("Transceiver is streaming. This operation is invalid during the track is in use.");
            if (Transceiver.Sender.Track.ReadyState == TrackState.Ended)
                throw new InvalidOperationException("Track has already been ended.");

            RTCErrorType error = Transceiver.SetCodec(new VideoCodecInfo[] { m_codec });
            if (error != RTCErrorType.None)
                throw new InvalidOperationException($"Set codec is failed. errorCode={error}");
        }

        private Texture m_receiveTexture;

        protected virtual void Start()
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
                    m_receiveTexture = texture;
                    OnUpdateReceiveTexture?.Invoke(m_receiveTexture);
                };
            }
        }

        private void StoppedStream(string connectionId)
        {
            m_receiveTexture = null;
            OnUpdateReceiveTexture?.Invoke(m_receiveTexture);
        }
    }
}
