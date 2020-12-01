using System;
using Unity.WebRTC;
using UnityEngine;
using UnityEngine.UI;

namespace Unity.RenderStreaming
{
    public class ReceiveVideoViewer : MonoBehaviour
    {
        [SerializeField] private Vector2Int streamingSize = new Vector2Int(1280, 720);
        [SerializeField] private Button sendOfferButton;
        [SerializeField] private RawImage receiveImage;

        private MediaStream m_receiveStream;

        void Start()
        {
            sendOfferButton.onClick.AddListener(() => RenderStreaming.Instance?.AddTransceiver());
        }

        void OnEnable()
        {
            m_receiveStream = new MediaStream();
            RenderStreaming.Instance?.AddVideoReceiveStream(m_receiveStream);
            m_receiveStream.OnAddTrack = e =>
            {
                if (receiveImage != null && e.Track.Kind == TrackKind.Video)
                {
                    var videoTrack = (VideoStreamTrack)e.Track;
                   receiveImage.texture = videoTrack.InitializeReceiver(streamingSize.x, streamingSize.y);
                }
            };
        }

        void OnDisable()
        {
            RenderStreaming.Instance?.RemoveVideoReceiveStream(m_receiveStream);
            m_receiveStream.OnAddTrack = null;
            m_receiveStream.Dispose();
            m_receiveStream = null;
        }
    }
}
