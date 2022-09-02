using System;
using Unity.WebRTC;
using UnityEngine;
using System.Linq;
using System.Collections.Generic;

namespace Unity.RenderStreaming
{
    /// <summary>
    ///
    /// </summary>
    [AddComponentMenu("Render Streaming/Video Stream Receiver")]
    public class VideoStreamReceiver : StreamReceiverBase
    {
        [SerializeField]
        private Texture m_targetTexture;

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
        public uint width
        {
            get { return (uint)m_targetTexture.width; }
        }

        /// <summary>
        /// 
        /// </summary>
        public uint height
        {
            get { return (uint)m_targetTexture.height; }
        }


        private VideoCodecInfo m_codec;

        private Texture m_receiveTexture;


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
        public Texture targetTexture => m_targetTexture;

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

            var codecs = new VideoCodecInfo[] { m_codec };
            RTCErrorType error = Transceiver.SetCodecPreferences(SelectCodecCapabilities(codecs).ToArray());
            if (error != RTCErrorType.None)
                throw new InvalidOperationException($"Set codec is failed. errorCode={error}");
        }

        internal IEnumerable<RTCRtpCodecCapability> SelectCodecCapabilities(IEnumerable<VideoCodecInfo> codecs)
        {
            return RTCRtpReceiver.GetCapabilities(TrackKind.Video).SelectCodecCapabilities(codecs);
        }

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
                    m_targetTexture = texture;
                    OnUpdateReceiveTexture?.Invoke(m_targetTexture);
                };
            }
        }

        private void StoppedStream(string connectionId)
        {
            m_targetTexture = null;
            OnUpdateReceiveTexture?.Invoke(m_targetTexture);
        }
    }
}
