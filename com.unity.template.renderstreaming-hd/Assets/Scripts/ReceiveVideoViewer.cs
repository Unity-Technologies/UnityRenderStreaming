using System;
using Unity.WebRTC;
using UnityEngine;
using UnityEngine.UI;

namespace Unity.RenderStreaming
{
    public class ReceiveVideoViewer : MonoBehaviour
    {
        [SerializeField] private Vector2Int streamingSize = new Vector2Int(1280, 720);
        [SerializeField] private InputField connectionIdInput;
        [SerializeField] private RawImage receiveImage;

        private MediaStream m_receiveStream;
        private string m_connectionId;

        void Awake()
        {
            m_connectionId = Guid.NewGuid().ToString();
            connectionIdInput.text = m_connectionId;
            connectionIdInput.onValueChanged.AddListener(input => m_connectionId = input);
        }

        void OnEnable()
        {
            m_receiveStream = new MediaStream();
            RenderStreaming.Instance?.AddVideoReceiveStream(this);
            // ToDo: need update webrtc package to 2.2
            m_receiveStream.OnAddTrack = e =>
            {
                if (receiveImage != null && e.Track.Kind == TrackKind.Video)
                {
                    var videoTrack = (VideoStreamTrack)e.Track;
                    receiveImage.texture = videoTrack.InitializeReceiver(streamingSize.x, streamingSize.y);
                }
            };

            RenderStreaming.Instance?.CreateConnection(m_connectionId, true);
        }

        void OnDisable()
        {
            RenderStreaming.Instance?.CloseConnection(m_connectionId);
            RenderStreaming.Instance?.RemoveVideoReceiveStream(this);
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
            if (connectionId != m_connectionId)
            {
                return;
            }

            m_receiveStream.AddTrack(trackEvent.Track);
        }
    }
}
