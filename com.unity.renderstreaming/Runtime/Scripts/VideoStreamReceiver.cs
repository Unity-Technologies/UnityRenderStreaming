using System;
using Unity.WebRTC;
using UnityEngine;

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
        /// argument index must use dictionary key from GetAvailableVideoCodecsName
        /// </summary>
        /// <seealso cref="AvailableCodecsUtils.GetAvailableVideoCodecsName"/>
        /// <param name="index"></param>
        /// <exception cref="ArgumentOutOfRangeException"></exception>
        public override void FilterCodecs(int index)
        {
            if (index < 0)
            {
                m_codecs.Clear();
                return;
            }

            if (!AvailableCodecsUtils.TryGetAvailableVideoCodec(index, out var codec))
            {
                throw new ArgumentOutOfRangeException(nameof(index), index, "Index was out of range.");
            }

            m_codecs.Clear();
            m_codecs.Add(codec);
        }

        /// <summary>
        ///
        /// </summary>
        [SerializeField] private Vector2Int streamingSize = new Vector2Int(1280, 720);

        /// <summary>
        ///
        /// </summary>
        public Texture ReceiveTexture => m_receiveTexture;

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
