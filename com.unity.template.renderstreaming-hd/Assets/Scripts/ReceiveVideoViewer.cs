using Unity.WebRTC;
using UnityEngine;

namespace Unity.RenderStreaming
{
    public class ReceiveVideoViewer : MonoBehaviour
    {
        public delegate void OnUpdateReceiveTextureHandler(Texture receiveTexture);
        public OnUpdateReceiveTextureHandler OnUpdateReceiveTexture;

        [SerializeField] private Vector2Int streamingSize = new Vector2Int(1280, 720);
        [SerializeField] private string connectionId;

        private MediaStream m_receiveStream;
        private Texture m_receiveTexture;

        public Texture ReceiveTexture => m_receiveTexture;

        void OnEnable()
        {
            m_receiveStream = new MediaStream();
            RenderStreaming.Instance?.AddVideoReceiveViewer(this);
            m_receiveStream.OnAddTrack = e =>
            {
                if (e.Track.Kind == TrackKind.Video)
                {
                    var videoTrack = (VideoStreamTrack)e.Track;
                    m_receiveTexture = videoTrack.InitializeReceiver(streamingSize.x, streamingSize.y);
                    OnUpdateReceiveTexture?.Invoke(m_receiveTexture);
                }
            };
            m_receiveStream.OnRemoveTrack = e =>
            {
                if (e.Track.Kind == TrackKind.Video)
                {
                    OnUpdateReceiveTexture?.Invoke(null);
                    m_receiveTexture = null;
                    e.Track.Dispose();
                }
            };

            RenderStreaming.Instance?.OpenConnection(connectionId);
        }

        void OnDisable()
        {
            RenderStreaming.Instance?.CloseConnection(connectionId);
            RenderStreaming.Instance?.RemoveVideoReceiveViewer(this);
            m_receiveStream.OnAddTrack = null;
            m_receiveStream.Dispose();
            m_receiveStream = null;
            m_receiveTexture = null;
        }

        public void AddTrack(string connectionId, MediaStreamTrack track)
        {
            if (!string.IsNullOrEmpty(this.connectionId) && connectionId != this.connectionId)
            {
                return;
            }

            m_receiveStream.AddTrack(track);
        }

        public void RemoveTrack(string connectionId, MediaStreamTrack track)
        {
            if (!string.IsNullOrEmpty(this.connectionId) && connectionId != this.connectionId)
            {
                return;
            }

            m_receiveStream.RemoveTrack(track);
        }

        public void ChangeConnectionId(string connectionId)
        {
            this.connectionId = connectionId;
        }
    }
}
