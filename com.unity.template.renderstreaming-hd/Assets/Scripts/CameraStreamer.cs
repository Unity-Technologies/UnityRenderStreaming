using System;
using Unity.WebRTC;
using UnityEngine;

namespace Unity.RenderStreaming
{
    [RequireComponent(typeof(Camera))]
    public class CameraStreamer : MonoBehaviour
    {
        [SerializeField, Tooltip("Streaming size should match display aspect ratio")]
        private Vector2Int streamingSize = new Vector2Int(1280, 720);

        private Camera m_camera;
        private VideoStreamTrack m_track;

        public void ChangeBitrate(int bitrate)
        {
            RenderStreaming.Instance?.ChangeVideoParameters(
                m_track, Convert.ToUInt64(bitrate), null);
        }

        public void ChangeFramerate(int framerate)
        {
            RenderStreaming.Instance?.ChangeVideoParameters(
                m_track, null, Convert.ToUInt32(framerate));
        }

        void Awake()
        {
            m_camera = GetComponent<Camera>();
        }

        void OnEnable()
        {
            // todo(kazuki): remove bitrate parameter because it is not supported
            m_track = m_camera.CaptureStreamTrack(streamingSize.x, streamingSize.y, 1000000);
            RenderStreaming.Instance?.AddVideoStreamTrack(m_track);
        }

        void OnDisable()
        {
            RenderStreaming.Instance?.RemoveVideoStreamTrack(m_track);
        }
    }
}
