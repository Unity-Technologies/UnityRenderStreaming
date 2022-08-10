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
