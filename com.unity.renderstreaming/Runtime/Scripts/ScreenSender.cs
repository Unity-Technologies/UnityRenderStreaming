using Unity.WebRTC;
using UnityEngine;
using UnityEngine.Experimental.Rendering;

namespace Unity.RenderStreaming
{
    public class ScreenSender : VideoStreamBase
    {
        [SerializeField] private int depth = 0;
        [SerializeField] private int antiAliasing = 1;

        public override Texture SendTexture => m_renderTexture;

        private RenderTexture m_renderTexture;

        protected virtual void Awake()
        {
            RenderTextureFormat supportFormat = WebRTC.WebRTC.GetSupportedRenderTextureFormat(SystemInfo.graphicsDeviceType);
            GraphicsFormat graphicsFormat = GraphicsFormatUtility.GetGraphicsFormat(supportFormat, RenderTextureReadWrite.Default);
            GraphicsFormat compatibleFormat = SystemInfo.GetCompatibleFormat(graphicsFormat, FormatUsage.Render);
            GraphicsFormat format = graphicsFormat == compatibleFormat ? graphicsFormat : compatibleFormat;

            m_renderTexture = new RenderTexture(Screen.width, Screen.height, depth, format);
        }

        protected override MediaStreamTrack CreateTrack()
        {
            return new VideoStreamTrack(m_renderTexture);
        }

        protected void Update()
        {
            ScreenCapture.CaptureScreenshotIntoRenderTexture(m_renderTexture);
        }
    }
}
