using Unity.WebRTC;
using UnityEngine;
using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using UnityEngine.Rendering;
using UnityEngine.Experimental.Rendering;

namespace Unity.RenderStreaming
{
    internal sealed class StreamingSizeAttribute : PropertyAttribute { }

    internal sealed class FrameRateAttribute : PropertyAttribute { }

    internal sealed class BitrateAttribute : PropertyAttribute
    {
        public int minValue;
        public int maxValue;

        public BitrateAttribute(int minValue, int maxValue)
        {
            this.minValue = minValue;
            this.maxValue = maxValue;
        }
    }

    internal sealed class RenderTextureAntiAliasingAttribute : PropertyAttribute { }

    internal sealed class RenderTextureDepthBufferAttribute : PropertyAttribute { }

    internal sealed class WebCamDeviceAttribute : PropertyAttribute { }

    internal sealed class ScaleResolutionAttribute : PropertyAttribute { }

    [Serializable]
    internal struct Range
    {
        public uint min;
        public uint max;

        public Range(uint min, uint max) { this.min = min; this.max = max; }
    }

    internal sealed class CodecAttribute : PropertyAttribute { }

    internal static class RTCRtpSenderExtension
    {
        public static RTCError SetFrameRate(this RTCRtpSender sender, uint framerate)
        {
            if (sender.Track.Kind != TrackKind.Video)
                throw new ArgumentException();

            RTCRtpSendParameters parameters = sender.GetParameters();
            foreach (var encoding in parameters.encodings)
            {
                encoding.maxFramerate = framerate;
            }
            return sender.SetParameters(parameters);
        }

        public static RTCError SetScaleResolutionDown(this RTCRtpSender sender, double? scaleFactor)
        {
            if (sender.Track.Kind != TrackKind.Video)
                throw new ArgumentException();

            RTCRtpSendParameters parameters = sender.GetParameters();
            foreach (var encoding in parameters.encodings)
            {
                encoding.scaleResolutionDownBy = scaleFactor;
            }
            return sender.SetParameters(parameters);
        }

        public static RTCError SetBitrate(this RTCRtpSender sender, uint? minBitrate, uint? maxBitrate)
        {
            RTCRtpSendParameters parameters = sender.GetParameters();

            foreach (var encoding in parameters.encodings)
            {
                encoding.minBitrate = minBitrate * 1000;
                encoding.maxBitrate = maxBitrate * 1000;
            }
            return sender.SetParameters(parameters);
        }
    }

    public enum VideoStreamSource
    {
        Camera = 0,
        Screen = 1,
        WebCamera = 2,
        Texture = 3
    }
    /// <summary>
    ///
    /// </summary>
    public class VideoStreamSender : StreamSenderBase
    {
        static readonly float s_defaultFrameRate = 30;

        static readonly uint s_defaultMinBitrate = 0;
        static readonly uint s_defaultMaxBitrate = 1000;

        static readonly int s_defaultDepth = 16;

        //todo(kazuki): remove this value.
        [SerializeField, StreamingSize]
        private Vector2Int m_textureSize = new Vector2Int(1280, 720);

        [SerializeField]
        private VideoStreamSource m_source;

        [SerializeField]
        private Camera m_camera;

        [SerializeField]
        private Texture m_texture;

        [SerializeField, WebCamDevice]
        private int m_webCamDeviceIndex;

        [SerializeField, RenderTextureDepthBuffer]
        private int m_depth = s_defaultDepth;

        [SerializeField, RenderTextureAntiAliasing]
        private int m_antiAliasing = 1;

        [SerializeField, Codec]
        private Codec<VideoStreamSender> m_codec;

        [SerializeField, FrameRate]
        private float m_frameRate = s_defaultFrameRate;

        [SerializeField, Bitrate(0, 10000)]
        private Range m_bitrate = new Range(s_defaultMinBitrate, s_defaultMaxBitrate);

        [SerializeField, ScaleResolution]
        private float m_scaleFactor = 1f;

        [SerializeField]
        private bool m_autoRequestUserAuthorization = true;

        private VideoStreamSourceImpl m_sourceImpl = null;

        /// <summary>
        /// 
        /// </summary>
        public VideoStreamSource source
        {
            get { return m_source; }
            set
            {
                m_source = value;
                if (m_texture != null)
                {
                    m_textureSize.x = m_texture.width;
                    m_textureSize.y = m_texture.height;
                }
                if (isPlaying)
                    ReplaceTrack(CreateTrack());
            }
        }

        /// <summary>
        /// 
        /// </summary>
        public Camera sourceCamera
        {
            get { return m_camera; }
            set
            {
                m_camera = value;
                if (isPlaying)
                    ReplaceTrack(CreateTrack());
            }
        }

        /// <summary>
        ///
        /// </summary>
        public Texture sourceTexture
        {
            get { return m_texture; }
            set
            {
                m_texture = value;
                m_textureSize.x = m_texture.width;
                m_textureSize.y = m_texture.height;
                if (isPlaying)
                    ReplaceTrack(CreateTrack());
            }
        }

        /// <summary>
        /// The index of WebCamTexture.devices.
        /// </summary>
        public int sourceDeviceIndex
        {
            get { return m_webCamDeviceIndex; }
            set
            {
                m_webCamDeviceIndex = value;
                if (isPlaying)
                    ReplaceTrack(CreateTrack());
            }
        }

        /// <summary>
        /// 
        /// </summary>
        public WebCamTexture sourceWebCamTexture
        {
            get
            {
                if(m_sourceImpl is VideoStreamSourceWebCam source)
                {
                    return source.webCamTexture;
                }
                return null;
            }
        }


        /// <summary>
        ///
        /// </summary>
        public float frameRate
        {
            get { return m_frameRate; }
        }

        /// <summary>
        ///
        /// </summary>
        public uint minBitrate
        {
            get { return m_bitrate.min; }
        }

        /// <summary>
        ///
        /// </summary>
        public uint maxBitrate
        {
            get { return m_bitrate.max; }
        }

        /// <summary>
        /// 
        /// </summary>
        public float scaleResolutionDown
        {
            get { return m_scaleFactor; }
        }

        /// <summary>
        /// 
        /// </summary>
        public uint width
        {
            get { return (uint)m_textureSize.x; }
            set
            {
                SetTextureSize(new Vector2Int((int)value, m_textureSize.y));
            }
        }

        /// <summary>
        /// 
        /// </summary>
        public uint height
        {
            get { return (uint)m_textureSize.y; }
            set
            {
                SetTextureSize(new Vector2Int(m_textureSize.x, (int)value));
            }
        }

        /// <summary>
        /// 
        /// </summary>
        public VideoCodecInfo codec
        {
            get { return (VideoCodecInfo)m_codec; }
        }


        public bool autoRequestUserAuthorization
        {
            get => m_autoRequestUserAuthorization;
            set { m_autoRequestUserAuthorization = value; }
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="mimeType"></param>
        public void SetCodec(VideoCodecInfo codec)
        {
            m_codec = (Codec<VideoStreamSender>)codec;
            foreach (var transceiver in Transceivers.Values)
            {
                if (!string.IsNullOrEmpty(transceiver.Mid))
                    continue;
                if (transceiver.Sender.Track.ReadyState == TrackState.Ended)
                    continue;

                RTCErrorType error = transceiver.SetCodec(new VideoCodecInfo[] { (VideoCodecInfo)m_codec });
                if (error != RTCErrorType.None)
                    throw new InvalidOperationException($"Set codec is failed. errorCode={error}");
            }
        }

        /// <summary>
        /// 
        /// </summary>
        /// <returns></returns>
        public static IEnumerable<VideoCodecInfo> GetAvailableCodecs()
        {
            string[] excludeCodecMimeType = { "video/red", "video/ulpfec", "video/rtx", "video/flexfec-03" };
            var capabilities = RTCRtpSender.GetCapabilities(TrackKind.Video);
            return capabilities.codecs.Where(codec => !excludeCodecMimeType.Contains(codec.mimeType)).Select(codec => VideoCodecInfo.Create(codec));
        }

        /// <summary>
        ///
        /// </summary>
        /// <param name="frameRate"></param>
        public void SetFrameRate(float frameRate)
        {
            if (frameRate < 0)
                throw new ArgumentOutOfRangeException("frameRate", frameRate, "The parameter must be greater than zero.");
            m_frameRate = frameRate;
            foreach (var transceiver in Transceivers.Values)
            {
                RTCError error = transceiver.Sender.SetFrameRate((uint)m_frameRate);
                if (error.errorType != RTCErrorType.None)
                    throw new InvalidOperationException($"Set framerate is failed. {error.message}");
            }
        }

        /// <summary>
        ///
        /// </summary>
        /// <param name="bitrate"></param>
        public void SetBitrate(uint minBitrate, uint maxBitrate)
        {
            if (minBitrate > maxBitrate)
                throw new ArgumentException("The maxBitrate must be greater than minBitrate.", "maxBitrate");
            m_bitrate.min = minBitrate;
            m_bitrate.max = maxBitrate;
            foreach (var transceiver in Transceivers.Values)
            {
                RTCError error = transceiver.Sender.SetBitrate(m_bitrate.min, m_bitrate.max);
                if (error.errorType != RTCErrorType.None)
                    throw new InvalidOperationException($"Set codec is failed. {error.message}");
            }
        }

        /// <summary>
        ///
        /// </summary>
        /// <param name="scaleFactor">The parameter must be greater than 1.0f.</param>
        public void SetScaleResolutionDown(float scaleFactor)
        {
            if (scaleFactor < 1.0f)
                throw new ArgumentOutOfRangeException("scaleFactor", scaleFactor, "The parameter must be greater than 1.0f. Scaleup is not allowed.");

            m_scaleFactor = scaleFactor;
            foreach (var transceiver in Transceivers.Values)
            {
                double? value = Mathf.Approximately(m_scaleFactor, 1) ? (double?)null : m_scaleFactor;
                RTCError error = transceiver.Sender.SetScaleResolutionDown(value);
                if (error.errorType != RTCErrorType.None)
                    throw new InvalidOperationException($"Set codec is failed. {error.message}");
            }
        }

        internal void SetTextureSize(Vector2Int size)
        {
            if (m_source == VideoStreamSource.Texture)
                throw new InvalidOperationException("Video source is set Texture.");
            m_textureSize = size;

            if (isPlaying)
                ReplaceTrack(CreateTrack());
        }

        internal override MediaStreamTrack CreateTrack()
        {
            m_sourceImpl?.Dispose();
            m_sourceImpl = CreateVideoStreamSource();
            return m_sourceImpl.CreateTrack();
        }

        VideoStreamSourceImpl CreateVideoStreamSource()
        {
            switch (m_source)
            {
                case VideoStreamSource.Camera:
                    return new VideoStreamSourceCamera(this);
                case VideoStreamSource.Screen:
                    return new VideoStreamSourceScreen(this);
                case VideoStreamSource.Texture:
                    return new VideoStreamSourceTexture(this);
                case VideoStreamSource.WebCamera:
                    return new VideoStreamSourceWebCam(this);
            }
            throw new InvalidOperationException("");
        }

        abstract class VideoStreamSourceImpl : IDisposable
        {
            public VideoStreamSourceImpl(VideoStreamSender parent)
            {
                width = (int)parent.width;
                height = (int)parent.height;
                depth = parent.m_depth;
                antiAliasing = parent.m_antiAliasing;
            }
            public abstract VideoStreamTrack CreateTrack();
            public abstract void Dispose();
            public int width { get; private set; }
            public int height { get; private set; }
            public int depth { get; private set; }
            public int antiAliasing { get; private set; }
        }

        class VideoStreamSourceCamera : VideoStreamSourceImpl
        {
            private Camera m_camera;
            private RenderTexture m_renderTexture;
            public VideoStreamSourceCamera(VideoStreamSender parent) : base(parent)
            {
                Camera camera = parent.m_camera;
                if (camera == null)
                    throw new ArgumentNullException("camera", "The sourceCamera is not assigned.");
                m_camera = camera;
            }

            public override VideoStreamTrack CreateTrack()
            {
                if (m_camera.targetTexture != null)
                {
                    m_renderTexture = m_camera.targetTexture;
                    RenderTextureFormat supportFormat = WebRTC.WebRTC.GetSupportedRenderTextureFormat(SystemInfo.graphicsDeviceType);
                    GraphicsFormat graphicsFormat = GraphicsFormatUtility.GetGraphicsFormat(supportFormat, RenderTextureReadWrite.Default);
                    GraphicsFormat compatibleFormat = SystemInfo.GetCompatibleFormat(graphicsFormat, FormatUsage.Render);
                    GraphicsFormat format = graphicsFormat == compatibleFormat ? graphicsFormat : compatibleFormat;

                    if (m_renderTexture.graphicsFormat != format)
                    {
                        Debug.LogWarning(
                            $"This color format:{m_renderTexture.graphicsFormat} not support in unity.webrtc. Change to supported color format:{format}.");
                        m_renderTexture.Release();
                        m_renderTexture.graphicsFormat = format;
                        m_renderTexture.Create();
                    }

                    m_camera.targetTexture = m_renderTexture;
                }
                else
                {
                    RenderTextureFormat format = WebRTC.WebRTC.GetSupportedRenderTextureFormat(SystemInfo.graphicsDeviceType);
                    m_renderTexture = new RenderTexture((int)width, (int)height, depth, format)
                    {
                        antiAliasing = antiAliasing
                    };
                    m_renderTexture.Create();
                    m_camera.targetTexture = m_renderTexture;
                }
                return new VideoStreamTrack(m_renderTexture);
            }

            public override void Dispose()
            {
                if (m_renderTexture == null)
                    return;
                m_camera.targetTexture = null;
                m_renderTexture.Release();
                Destroy(m_renderTexture);
                m_renderTexture = null;

                GC.SuppressFinalize(this);
            }

            ~VideoStreamSourceCamera()
            {
                Dispose();
            }
        }

        class VideoStreamSourceTexture : VideoStreamSourceImpl
        {
            Texture m_texture;
            RenderTexture m_copyTexture;
            private Coroutine m_coroutineScreenCapture;
            private MonoBehaviour m_behaviour;

            public VideoStreamSourceTexture(VideoStreamSender parent) : base(parent)
            {
                Texture texture = parent.m_texture;
                if (texture == null)
                    throw new ArgumentNullException("texture", "The sourceTexture is not assigned.");
                m_texture = texture;
                m_behaviour = parent;
            }

            public override VideoStreamTrack CreateTrack()
            {
                GraphicsFormat format =
                    WebRTC.WebRTC.GetSupportedGraphicsFormat(SystemInfo.graphicsDeviceType);
                if (m_texture.graphicsFormat == format)
                    return new VideoStreamTrack(m_texture);
                m_copyTexture = new RenderTexture(width, height, depth, format) { antiAliasing = antiAliasing };
                m_copyTexture.Create();

                m_coroutineScreenCapture = m_behaviour.StartCoroutine(RecordScreenFrame());
                return new VideoStreamTrack(m_copyTexture);
            }

            public override void Dispose()
            {
                if (m_copyTexture != null)
                {
                    Destroy(m_copyTexture);
                    m_copyTexture = null;
                }

                if (m_coroutineScreenCapture != null)
                {
                    m_behaviour.StopCoroutine(m_coroutineScreenCapture);
                    m_behaviour = null;
                    m_coroutineScreenCapture = null;
                }
                GC.SuppressFinalize(this);
            }

            ~VideoStreamSourceTexture()
            {
                Dispose();
            }

            IEnumerator RecordScreenFrame()
            {
                while (true)
                {
                    yield return new WaitForEndOfFrame();

                    Graphics.Blit(m_texture, m_copyTexture);
                }
            }
        }

        class VideoStreamSourceScreen : VideoStreamSourceImpl, IDisposable
        {
            private RenderTexture m_screenTexture;
            private RenderTexture m_screenCopyTexture;
            private Coroutine m_coroutineScreenCapture;
            private MonoBehaviour m_behaviour;

            public VideoStreamSourceScreen(VideoStreamSender parent) : base(parent)
            {
                m_behaviour = parent;
            }

            static Vector2Int GetScreenSize()
            {
                /// Screen.width/height returns size of the active window.
                /// However, it is mandatory to get size of the game view when player mode.
                /// UnityStats is used here because it returns the size of game view anytime.
#if UNITY_EDITOR
                string[] screenres = UnityEditor.UnityStats.screenRes.Split('x');
                int screenWidth = int.Parse(screenres[0]);
                int screenHeight = int.Parse(screenres[1]);

                /// Set Screen.width/height forcely because UnityStats returns zero when batch mode.
                if (screenWidth == 0 || screenHeight == 0)
                {
                    screenWidth = Screen.width;
                    screenHeight = Screen.height;
                }
#else
                int screenWidth = Screen.width;
                int screenHeight = Screen.height;
#endif
                return new Vector2Int(screenWidth, screenHeight);
            }

            public override VideoStreamTrack CreateTrack()
            {
                Vector2Int screenSize = GetScreenSize();
                m_screenTexture =
                    new RenderTexture(screenSize.x, screenSize.y, depth, RenderTextureFormat.Default) { antiAliasing = antiAliasing };
                m_screenTexture.Create();

                RenderTextureFormat format =
                    WebRTC.WebRTC.GetSupportedRenderTextureFormat(SystemInfo.graphicsDeviceType);
                m_screenCopyTexture = new RenderTexture(width, height, depth, format) { antiAliasing = antiAliasing };
                m_screenCopyTexture.Create();

                // The texture obtained by ScreenCapture.CaptureScreenshotIntoRenderTexture is different between OpenGL and other Graphics APIs.
                // In OpenGL, we got a texture that is not inverted, so need flip when sending.
                var isOpenGl = SystemInfo.graphicsDeviceType == GraphicsDeviceType.OpenGLCore ||
                               SystemInfo.graphicsDeviceType == GraphicsDeviceType.OpenGLES2 ||
                               SystemInfo.graphicsDeviceType == GraphicsDeviceType.OpenGLES3;

                m_coroutineScreenCapture = m_behaviour.StartCoroutine(RecordScreenFrame());
                return new VideoStreamTrack(m_screenCopyTexture, isOpenGl);
            }

            IEnumerator RecordScreenFrame()
            {
                while (true)
                {
                    yield return new WaitForEndOfFrame();
                    ScreenCapture.CaptureScreenshotIntoRenderTexture(m_screenTexture);
                    Graphics.Blit(m_screenTexture, m_screenCopyTexture);
                }
            }

            public override void Dispose()
            {
                if (m_screenTexture == null)
                    return;
                m_screenTexture.Release();
                Destroy(m_screenTexture);
                m_screenTexture = null;

                m_screenCopyTexture.Release();
                Destroy(m_screenCopyTexture);
                m_screenCopyTexture = null;

                m_behaviour.StopCoroutine(m_coroutineScreenCapture);
                m_behaviour = null;
                m_coroutineScreenCapture = null;

                GC.SuppressFinalize(this);
            }

            ~VideoStreamSourceScreen()
            {
                Dispose();
            }
        }

        class VideoStreamSourceWebCam : VideoStreamSourceImpl, IDisposable
        {
            int m_deviceIndex;
            bool m_autoRequestUserAuthorization;
            float m_frameRate;
            WebCamTexture m_webcamTexture;

            public WebCamTexture webCamTexture => m_webcamTexture;

            public VideoStreamSourceWebCam(VideoStreamSender parent) : base(parent)
            {
                int deviceIndex = parent.m_webCamDeviceIndex;
                if (deviceIndex < 0 || WebCamTexture.devices.Length <= deviceIndex)
                    throw new ArgumentOutOfRangeException("deviceIndex", deviceIndex, "The deviceIndex is out of range");
                m_deviceIndex = deviceIndex;
                m_frameRate = parent.m_frameRate;
                m_autoRequestUserAuthorization = parent.m_autoRequestUserAuthorization;
            }

            public override VideoStreamTrack CreateTrack()
            {
                if (m_autoRequestUserAuthorization)
                {
                    AsyncOperation op = Application.RequestUserAuthorization(UserAuthorization.WebCam);
                    while (!op.isDone)
                    {
                        System.Threading.Thread.Sleep(1);
                    }
                }
                if (!Application.HasUserAuthorization(UserAuthorization.WebCam))
                    throw new InvalidOperationException("Call Application.RequestUserAuthorization before creating track with WebCam.");

                WebCamDevice userCameraDevice = WebCamTexture.devices[m_deviceIndex];
                m_webcamTexture = new WebCamTexture(userCameraDevice.name, (int)width, (int)height, (int)m_frameRate);
                m_webcamTexture.Play();

                return new VideoStreamTrack(m_webcamTexture);
            }

            public override void Dispose()
            {
                if (m_webcamTexture == null)
                    return;
                m_webcamTexture.Stop();
                Destroy(m_webcamTexture);
                m_webcamTexture = null;

                GC.SuppressFinalize(this);
            }

            ~VideoStreamSourceWebCam()
            {
                Dispose();
            }
        }
    }
}
