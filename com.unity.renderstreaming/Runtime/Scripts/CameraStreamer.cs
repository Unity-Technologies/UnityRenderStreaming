using Unity.WebRTC;
using UnityEngine;
using UnityEngine.Experimental.Rendering;

namespace Unity.RenderStreaming
{
    [RequireComponent(typeof(Camera))]
    public class CameraStreamer : VideoStreamBase
    {
        [SerializeField] private int depth = 0;
        [SerializeField] private int antiAliasing = 1;


        private Camera m_camera;
        public override Texture SendTexture => m_camera.targetTexture;

        protected virtual void Awake()
        {
            m_camera = GetComponent<Camera>();
        }

        protected override MediaStreamTrack CreateTrack()
        {
            RenderTexture rt;
            if (m_camera.targetTexture != null)
            {
                rt = m_camera.targetTexture;
                RenderTextureFormat supportFormat = WebRTC.WebRTC.GetSupportedRenderTextureFormat(SystemInfo.graphicsDeviceType);
                GraphicsFormat graphicsFormat = GraphicsFormatUtility.GetGraphicsFormat(supportFormat, RenderTextureReadWrite.Default);
                GraphicsFormat compatibleFormat = SystemInfo.GetCompatibleFormat(graphicsFormat, FormatUsage.Render);
                GraphicsFormat format = graphicsFormat == compatibleFormat ? graphicsFormat : compatibleFormat;

                if (rt.graphicsFormat != format)
                {
                    Debug.LogWarning(
                        $"This color format:{rt.graphicsFormat} not support in unity.webrtc. Change to supported color format:{format}.");
                    rt.Release();
                    rt.graphicsFormat = format;
                    rt.Create();
                }

                m_camera.targetTexture = rt;
            }
            else
            {
                RenderTextureFormat format = WebRTC.WebRTC.GetSupportedRenderTextureFormat(SystemInfo.graphicsDeviceType);
                rt = new RenderTexture(streamingSize.x, streamingSize.y, depth, format)
                {
                    antiAliasing = antiAliasing
                };
                rt.Create();
                m_camera.targetTexture = rt;
            }

            return new VideoStreamTrack(m_camera.name, rt);
        }
    }
}
