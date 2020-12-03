using Unity.WebRTC;
using UnityEngine;
using UnityEngine.UI;

namespace Unity.RenderStreaming
{
    public class ReceiveVideoViewer : MonoBehaviour
    {
        [SerializeField] private Vector2Int streamingSize = new Vector2Int(1280, 720);
        [SerializeField] private string connectionId;
        [SerializeField] private RawImage receiveImage;

        private MediaStream m_receiveStream;

        void OnEnable()
        {
            m_receiveStream = new MediaStream();
            RenderStreaming.Instance?.AddVideoReceiveViewer(this);
            m_receiveStream.OnAddTrack = e =>
            {
                if (receiveImage != null && e.Track.Kind == TrackKind.Video)
                {
                    var videoTrack = (VideoStreamTrack)e.Track;
                    receiveImage.texture = videoTrack.InitializeReceiver(streamingSize.x, streamingSize.y);
                }
            };
            m_receiveStream.OnRemoveTrack = e =>
            {
                if (receiveImage != null && e.Track.Kind == TrackKind.Video)
                {
                    receiveImage.texture = null;
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

            if (receiveImage != null)
            {
                receiveImage.texture = null;
            }
        }

        public void AddTrack(string connectionId, RTCTrackEvent trackEvent)
        {
            if (connectionId != this.connectionId)
            {
                return;
            }

            m_receiveStream.AddTrack(trackEvent.Track);
        }

        public void RemoveTrack(string connectionId, MediaStreamTrack track)
        {
            if (connectionId != this.connectionId)
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
