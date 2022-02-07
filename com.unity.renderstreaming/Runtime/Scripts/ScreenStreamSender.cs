using System.Collections;
using System.Collections.Generic;
using System.Linq;
using Unity.WebRTC;
using UnityEngine;
using UnityEngine.Experimental.Rendering;
using UnityEngine.Rendering;

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
        private HashSet<string> connections = new HashSet<string>();

        protected virtual void Awake()
        {
            var format = WebRTC.WebRTC.GetSupportedRenderTextureFormat(SystemInfo.graphicsDeviceType);
            m_screenTexture =
                new RenderTexture(Screen.width, Screen.height, depth, RenderTextureFormat.Default) { antiAliasing = antiAliasing };
            m_screenTexture.Create();

            StartCoroutine(RecordScreenFrame());

            OnStartedStream += id => connections.Add(id);
            OnStoppedStream += id => connections.Remove(id);
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

            // The texture obtained by ScreenCapture.CaptureScreenshotIntoRenderTexture is different between OpenGL and other Graphics APIs.
            // In OpenGL, we got a texture that is not inverted, so need flip when sending.
            var isOpenGl = SystemInfo.graphicsDeviceType == GraphicsDeviceType.OpenGLCore ||
                           SystemInfo.graphicsDeviceType == GraphicsDeviceType.OpenGLES2 ||
                           SystemInfo.graphicsDeviceType == GraphicsDeviceType.OpenGLES3;

            return new VideoStreamTrack(rt, isOpenGl);
        }

        IEnumerator RecordScreenFrame()
        {
            while (true)
            {
                yield return new WaitForEndOfFrame();

                if (!connections.Any() || m_sendTexture == null || !m_sendTexture.IsCreated())
                {
                    continue;
                }

                ScreenCapture.CaptureScreenshotIntoRenderTexture(m_screenTexture);
                Graphics.Blit(m_screenTexture, m_sendTexture);
            }
        }
    }
}
