using Unity.WebRTC;
using UnityEngine;

namespace Unity.RenderStreaming
{
    [RequireComponent(typeof(Camera))]
    public class CameraStreamer : VideoStreamBase
    {
        [SerializeField] private RenderTextureDepth depth;
        [SerializeField, Tooltip("This property is needed to choose from 1,2,4 or 8")]
        private int antiAliasing = 1; //ToDO(kannan):using enum


        protected Camera m_camera;
        public override Texture SendTexture => m_camera.targetTexture;

        protected virtual void Awake()
        {
            m_camera = GetComponent<Camera>();
        }

        protected override MediaStreamTrack CreateTrack()
        {
            int depthValue = (int)depth;
            var format = WebRTC.WebRTC.GetSupportedRenderTextureFormat(SystemInfo.graphicsDeviceType);
            var rt = new RenderTexture(streamingSize.x, streamingSize.y, depthValue, format)
            {
                antiAliasing = antiAliasing
            };
            rt.Create();
            m_camera.targetTexture = rt;
            return new VideoStreamTrack(m_camera.name, rt);
        }
    }
}
