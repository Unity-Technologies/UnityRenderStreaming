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
        private RenderTexture m_screenTexture;

        protected virtual void Awake()
        {
            var format = WebRTC.WebRTC.GetSupportedRenderTextureFormat(SystemInfo.graphicsDeviceType);
            m_screenTexture =
                new RenderTexture(Screen.width, Screen.height, depth, format) { antiAliasing = antiAliasing };
            m_screenTexture.Create();
        }

        protected void OnDestroy()
        {
            if (m_sendTexture != null)
            {
                DestroyImmediate(m_sendTexture);
                m_sendTexture = null;
            }

            if (m_screenTexture != null)
            {
                DestroyImmediate(m_screenTexture);
                m_screenTexture = null;
            }
        }

        protected override MediaStreamTrack CreateTrack()
        {
            RenderTexture rt;
            if (m_sendTexture != null)
            {
                rt = m_sendTexture;
                RenderTextureFormat supportFormat =
                    WebRTC.WebRTC.GetSupportedRenderTextureFormat(SystemInfo.graphicsDeviceType);
                GraphicsFormat graphicsFormat =
                    GraphicsFormatUtility.GetGraphicsFormat(supportFormat, RenderTextureReadWrite.Default);
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

                m_sendTexture = rt;
            }
            else
            {
                RenderTextureFormat format =
                    WebRTC.WebRTC.GetSupportedRenderTextureFormat(SystemInfo.graphicsDeviceType);
                rt = new RenderTexture(streamingSize.x, streamingSize.y, depth, format) { antiAliasing = antiAliasing };
                rt.Create();
                m_sendTexture = rt;
            }

            return new VideoStreamTrack(rt.GetNativeTexturePtr(), rt.width, rt.height, rt.graphicsFormat);
        }

        protected void LateUpdate()
        {
            if (m_screenTexture != null && m_screenTexture.IsCreated())
            {
                StartCoroutine(RecordFrame());
            }
        }

        IEnumerator RecordFrame()
        {
            yield return new WaitForEndOfFrame();
            ScreenCapture.CaptureScreenshotIntoRenderTexture(m_screenTexture);

            if (m_sendTexture != null && m_sendTexture.IsCreated())
            {
                Graphics.Blit(m_screenTexture, m_sendTexture);
            }
        }
    }
}
