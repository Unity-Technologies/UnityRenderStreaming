using System.Collections;
using Unity.WebRTC;
using UnityEngine;
using UnityEngine.Experimental.Rendering;

namespace Unity.RenderStreaming
{
    /// <summary>
    /// 
    /// </summary>
    public class ScreenStreamSender : VideoStreamSender
    {
        /// <summary>
        /// 
        /// </summary>
        [SerializeField, RenderTextureDepthBuffer]
        private int depth = 0;

        /// <summary>
        /// 
        /// </summary>
        [SerializeField, RenderTextureAntiAliasing]
        private int antiAliasing = 1;

        public override Texture SendTexture => m_sendTexture;
        private RenderTexture m_sendTexture;

        protected virtual void Awake()
        {
            RenderTextureFormat supportFormat =
                WebRTC.WebRTC.GetSupportedRenderTextureFormat(SystemInfo.graphicsDeviceType);
            GraphicsFormat graphicsFormat =
                GraphicsFormatUtility.GetGraphicsFormat(supportFormat, RenderTextureReadWrite.Default);
            GraphicsFormat compatibleFormat = SystemInfo.GetCompatibleFormat(graphicsFormat, FormatUsage.Render);
            GraphicsFormat format = graphicsFormat == compatibleFormat ? graphicsFormat : compatibleFormat;

            m_sendTexture =
                new RenderTexture(Screen.width, Screen.height, depth, format) { antiAliasing = antiAliasing };
        }

        protected override MediaStreamTrack CreateTrack()
        {
            return new VideoStreamTrack(m_sendTexture.GetNativeTexturePtr(), m_sendTexture.width, m_sendTexture.height, m_sendTexture.graphicsFormat);
        }

        protected void LateUpdate()
        {
            StartCoroutine(RecordFrame());
        }

        IEnumerator RecordFrame()
        {
            yield return new WaitForEndOfFrame();
            ScreenCapture.CaptureScreenshotIntoRenderTexture(m_sendTexture);
        }
    }
}
