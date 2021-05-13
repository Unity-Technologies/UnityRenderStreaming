using Unity.WebRTC;
using UnityEngine;

namespace Unity.RenderStreaming
{
    /// <summary>
    /// 
    /// </summary>
    public class ReceiveVideoViewer : StreamReceiverBase
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

        private Texture m_receiveTexture;

        protected virtual void Start()
        {
            OnStartedStream += StartedStream;
            OnStoppedStream += StoppedStream;
        }

        private void StartedStream(string connectionId)
        {
            var videoTrack = Track as VideoStreamTrack;
            m_receiveTexture = videoTrack?.InitializeReceiver(streamingSize.x, streamingSize.y);
            OnUpdateReceiveTexture?.Invoke(m_receiveTexture);
        }

        private void StoppedStream(string connectionId)
        {
            m_receiveTexture = null;
            OnUpdateReceiveTexture?.Invoke(m_receiveTexture);
        }
    }
}
