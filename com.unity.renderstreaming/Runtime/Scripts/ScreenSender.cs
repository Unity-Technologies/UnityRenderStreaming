using System.Collections;
using Unity.WebRTC;
using UnityEngine;
using UnityEngine.Experimental.Rendering;
using UnityEngine.Rendering;

namespace Unity.RenderStreaming
{
    public class ScreenSender : VideoStreamBase
    {
        [SerializeField] private int depth = 0;
        [SerializeField] private int antiAliasing = 1;

        public override Texture SendTexture => m_renderTexture;

        private RenderTexture m_renderTexture;
        private RenderTexture m_sendTexture;
        private Material flipMat;

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
            m_renderTexture =
                new RenderTexture(Screen.width, Screen.height, depth, format) { antiAliasing = antiAliasing };

            var flipShader = Resources.Load<Shader>("Flip");
            if (flipShader != null)
            {
                flipMat = new Material(flipShader);
            }
        }

        protected override MediaStreamTrack CreateTrack()
        {
            return new VideoStreamTrack(m_sendTexture);
        }

        protected void LateUpdate()
        {
            StartCoroutine(RecordFrame());
        }

        IEnumerator RecordFrame()
        {
            yield return new WaitForEndOfFrame();
            ScreenCapture.CaptureScreenshotIntoRenderTexture(m_renderTexture);
            // ScreenCapture result need flip
            // if expose need flip property on VideoStreamTrack, this process no need.
            Graphics.Blit(m_renderTexture, m_sendTexture, flipMat);
        }
    }
}
