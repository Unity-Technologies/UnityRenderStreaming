using Unity.WebRTC;
using UnityEngine;

namespace Unity.RenderStreaming
{
    [RequireComponent(typeof(Camera))]
    public class CameraStreamer : VideoStreamBase
    {
        private Camera m_camera;
        public override Texture SendTexture => m_camera.targetTexture;

        protected virtual void Awake()
        {
            m_camera = GetComponent<Camera>();
        }

        protected override MediaStreamTrack CreateTrack()
        {
            return m_camera.CaptureStreamTrack(streamingSize.x, streamingSize.y, 1000000);
        }
    }
}
