using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using Unity.WebRTC;
using UnityEngine;
using UnityEngine.Experimental.Rendering;
using UnityEngine.Rendering;

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

    /// <summary>
    ///
    /// </summary>
    public enum VideoStreamSource
    {
        /// <summary>
        ///
        /// </summary>
        Camera = 0,
        /// <summary>
        ///
        /// </summary>
        Screen = 1,
        /// <summary>
        ///
        /// </summary>
        WebCamera = 2,
        /// <summary>
        ///
        /// </summary>
        Texture = 3
    }
    /// <summary>
    ///
    /// </summary>
    [AddComponentMenu("Render Streaming/Video Stream Sender")]
    public class VideoStreamSender : StreamSenderBase
    {
        static readonly float s_defaultFrameRate = 30;
        static readonly uint s_defaultMinBitrate = 0;
        static readonly uint s_defaultMaxBitrate = 1000;
        static readonly int s_defaultDepth = 16;

        internal const string SourcePropertyName = nameof(m_Source);
        internal const string CameraPropertyName = nameof(m_Camera);
        internal const string TexturePropertyName = nameof(m_Texture);
        internal const string WebCamDeviceIndexPropertyName = nameof(m_WebCamDeviceIndex);
        internal const string CodecPropertyName = nameof(m_Codec);
        internal const string TextureSizePropertyName = nameof(m_TextureSize);
        internal const string FrameRatePropertyName = nameof(m_FrameRate);
        internal const string BitratePropertyName = nameof(m_Bitrate);
        internal const string ScaleFactorPropertyName = nameof(m_ScaleFactor);
        internal const string DepthPropertyName = nameof(m_Depth);
        internal const string AntiAliasingPropertyName = nameof(m_AntiAliasing);
        internal const string AutoRequestUserAuthorizationPropertyName = nameof(m_AutoRequestUserAuthorization);

        //todo(kazuki): remove this value.
        [SerializeField, StreamingSize]
        private Vector2Int m_TextureSize = new Vector2Int(1280, 720);

        [SerializeField]
        private VideoStreamSource m_Source;

        [SerializeField]
        private Camera m_Camera;

        [SerializeField]
        private Texture m_Texture;

        [SerializeField, WebCamDevice]
        private int m_WebCamDeviceIndex;

        [SerializeField, RenderTextureDepthBuffer]
        private int m_Depth = s_defaultDepth;

        [SerializeField, RenderTextureAntiAliasing]
        private int m_AntiAliasing = 1;

        [SerializeField, Codec]
        private VideoCodecInfo m_Codec;

        [SerializeField, FrameRate]
        private float m_FrameRate = s_defaultFrameRate;

        [SerializeField, Bitrate(0, 10000)]
        private Range m_Bitrate = new Range(s_defaultMinBitrate, s_defaultMaxBitrate);

        [SerializeField, ScaleResolution]
        private float m_ScaleFactor = 1f;

        [SerializeField]
        private bool m_AutoRequestUserAuthorization = true;

        private VideoStreamSourceImpl m_sourceImpl = null;

        /// <summary>
        ///
        /// </summary>
        public VideoStreamSource source
        {
            get { return m_Source; }
            set
            {
                if (m_Source == value)
                    return;
                m_Source = value;

                if (!isPlaying)
                    return;

                var op = CreateTrack();
                StartCoroutineWithCallback(op, _ => ReplaceTrack(_.Track));
            }
        }

        /// <summary>
        ///
        /// </summary>
        public Camera sourceCamera
        {
            get { return m_Camera; }
            set
            {
                if (m_Camera == value)
                    return;
                m_Camera = value;

                if (!isPlaying || m_Source != VideoStreamSource.Camera)
                    return;

                var op = CreateTrack();
                StartCoroutineWithCallback(op, _ => ReplaceTrack(_.Track));
            }
        }

        /// <summary>
        ///
        /// </summary>
        public Texture sourceTexture
        {
            get { return m_Texture; }
            set
            {
                if (m_Texture == value)
                    return;
                m_Texture = value;

                if (!isPlaying || m_Source != VideoStreamSource.Texture)
                    return;

                var op = CreateTrack();
                StartCoroutineWithCallback(op, _ => ReplaceTrack(_.Track));
            }
        }

        /// <summary>
        /// The index of WebCamTexture.devices.
        /// </summary>
        public int sourceDeviceIndex
        {
            get { return m_WebCamDeviceIndex; }
            set
            {
                if (m_WebCamDeviceIndex == value)
                    return;
                m_WebCamDeviceIndex = value;

                if (!isPlaying || m_Source != VideoStreamSource.WebCamera)
                    return;

                var op = CreateTrack();
                StartCoroutineWithCallback(op, _ => ReplaceTrack(_.Track));
            }
        }

        /// <summary>
        ///
        /// </summary>
        public WebCamTexture sourceWebCamTexture
        {
            get
            {
                if (m_sourceImpl is VideoStreamSourceWebCam source)
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
            get { return m_FrameRate; }
        }

        /// <summary>
        ///
        /// </summary>
        public uint minBitrate
        {
            get { return m_Bitrate.min; }
        }

        /// <summary>
        ///
        /// </summary>
        public uint maxBitrate
        {
            get { return m_Bitrate.max; }
        }

        /// <summary>
        ///
        /// </summary>
        public float scaleResolutionDown
        {
            get { return m_ScaleFactor; }
        }

        /// <summary>
        ///
        /// </summary>
        public uint width
        {
            get { return (uint)m_TextureSize.x; }
            set
            {
                SetTextureSize(new Vector2Int((int)value, m_TextureSize.y));
            }
        }

        /// <summary>
        ///
        /// </summary>
        public uint height
        {
            get { return (uint)m_TextureSize.y; }
            set
            {
                SetTextureSize(new Vector2Int(m_TextureSize.x, (int)value));
            }
        }

        /// <summary>
        ///
        /// </summary>
        public VideoCodecInfo codec
        {
            get { return m_Codec; }
        }

        /// <summary>
        ///
        /// </summary>
        public bool autoRequestUserAuthorization
        {
            get => m_AutoRequestUserAuthorization;
            set { m_AutoRequestUserAuthorization = value; }
        }

        /// <summary>
        ///
        /// </summary>
        /// <param name="codec"></param>
        public void SetCodec(VideoCodecInfo codec)
        {
            if (isPlaying)
                throw new InvalidOperationException("Can not change this parameter after the streaming is started.");

            m_Codec = codec;
            foreach (var transceiver in Transceivers.Values)
            {
                if (!string.IsNullOrEmpty(transceiver.Mid))
                    continue;
                if (transceiver.Sender.Track.ReadyState == TrackState.Ended)
                    continue;

                var codecs = new[] { m_Codec };
                RTCErrorType error = transceiver.SetCodecPreferences(SelectCodecCapabilities(codecs).ToArray());
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
            m_FrameRate = frameRate;
            foreach (var transceiver in Transceivers.Values)
            {
                RTCError error = transceiver.Sender.SetFrameRate((uint)m_FrameRate);
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
            m_Bitrate.min = minBitrate;
            m_Bitrate.max = maxBitrate;
            foreach (var transceiver in Transceivers.Values)
            {
                RTCError error = transceiver.Sender.SetBitrate(m_Bitrate.min, m_Bitrate.max);
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

            m_ScaleFactor = scaleFactor;
            foreach (var transceiver in Transceivers.Values)
            {
                double? value = Mathf.Approximately(m_ScaleFactor, 1) ? (double?)null : m_ScaleFactor;
                RTCError error = transceiver.Sender.SetScaleResolutionDown(value);
                if (error.errorType != RTCErrorType.None)
                    throw new InvalidOperationException($"Set codec is failed. {error.message}");
            }
        }

        /// <summary>
        ///
        /// </summary>
        /// <param name="size"></param>
        public void SetTextureSize(Vector2Int size)
        {
            m_TextureSize = size;

            if (!isPlaying)
                return;

            var op = CreateTrack();
            StartCoroutineWithCallback(op, _ => ReplaceTrack(_.Track));
        }

        private protected virtual void Awake()
        {
            OnStoppedStream += _OnStoppedStream;
        }

        private protected override void OnDestroy()
        {
            base.OnDestroy();

            m_sourceImpl?.Dispose();
            m_sourceImpl = null;
        }

        void _OnStoppedStream(string connectionId)
        {
            m_sourceImpl?.Dispose();
            m_sourceImpl = null;
        }

        internal override WaitForCreateTrack CreateTrack()
        {
            m_sourceImpl?.Dispose();
            m_sourceImpl = CreateVideoStreamSource();
            return m_sourceImpl.CreateTrack();
        }

        VideoStreamSourceImpl CreateVideoStreamSource()
        {
            switch (m_Source)
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
                depth = parent.m_Depth;
                antiAliasing = parent.m_AntiAliasing;
            }
            public abstract WaitForCreateTrack CreateTrack();
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
                Camera camera = parent.m_Camera;
                if (camera == null)
                    throw new ArgumentNullException("camera", "The sourceCamera is not assigned.");
                m_camera = camera;
            }

            public override WaitForCreateTrack CreateTrack()
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
                        RenderStreaming.Logger.Log(LogType.Warning,
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
                var instruction = new WaitForCreateTrack();
                instruction.Done(new VideoStreamTrack(m_renderTexture));
                return instruction;
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
            Texture m_copyTexture;
            private Coroutine m_coroutineScreenCapture;
            private MonoBehaviour m_behaviour;

            public VideoStreamSourceTexture(VideoStreamSender parent) : base(parent)
            {
                Texture texture = parent.m_Texture;
                if (texture == null)
                    throw new ArgumentNullException("texture", "The sourceTexture is not assigned.");
                m_texture = texture;
                m_behaviour = parent;
            }

            public override WaitForCreateTrack CreateTrack()
            {
                var instruction = new WaitForCreateTrack();

                GraphicsFormat format =
                    WebRTC.WebRTC.GetSupportedGraphicsFormat(SystemInfo.graphicsDeviceType);
                if (m_texture.graphicsFormat == format && m_texture.width == width && m_texture.height == height)
                {
                    instruction.Done(new VideoStreamTrack(m_texture));
                    return instruction;
                }

                m_copyTexture = new Texture2D(width, height, format, TextureCreationFlags.None);
                m_coroutineScreenCapture = m_behaviour.StartCoroutine(RecordScreenFrame());
                instruction.Done(new VideoStreamTrack(m_copyTexture));
                return instruction;
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

                    Graphics.ConvertTexture(m_texture, m_copyTexture);
                }
            }
        }

        class VideoStreamSourceScreen : VideoStreamSourceImpl, IDisposable
        {
            private RenderTexture m_screenTexture;
            private Texture m_screenCopyTexture;
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

            public override WaitForCreateTrack CreateTrack()
            {
                Vector2Int screenSize = GetScreenSize();
                m_screenTexture =
                    new RenderTexture(screenSize.x, screenSize.y, depth, RenderTextureFormat.Default) { antiAliasing = antiAliasing };
                m_screenTexture.Create();

                GraphicsFormat format = WebRTC.WebRTC.GetSupportedGraphicsFormat(SystemInfo.graphicsDeviceType);
                m_screenCopyTexture = new Texture2D(width, height, format, TextureCreationFlags.None);

                // The texture obtained by ScreenCapture.CaptureScreenshotIntoRenderTexture is different between OpenGL and other Graphics APIs.
                // In OpenGL, we got a texture that is not inverted, so need flip when sending.
                var isOpenGl = SystemInfo.graphicsDeviceType == GraphicsDeviceType.OpenGLCore ||
                               SystemInfo.graphicsDeviceType == GraphicsDeviceType.OpenGLES2 ||
                               SystemInfo.graphicsDeviceType == GraphicsDeviceType.OpenGLES3;

                m_coroutineScreenCapture = m_behaviour.StartCoroutine(RecordScreenFrame());
                var instruction = new WaitForCreateTrack();
                instruction.Done(new VideoStreamTrack(m_screenCopyTexture, isOpenGl));
                return instruction;
            }

            IEnumerator RecordScreenFrame()
            {
                while (true)
                {
                    yield return new WaitForEndOfFrame();
                    ScreenCapture.CaptureScreenshotIntoRenderTexture(m_screenTexture);
                    Graphics.ConvertTexture(m_screenTexture, m_screenCopyTexture);
                }
            }

            public override void Dispose()
            {
                if (m_screenTexture == null)
                    return;
                m_screenTexture.Release();
                Destroy(m_screenTexture);
                m_screenTexture = null;

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
            Texture m_webcamCopyTexture;
            VideoStreamSender m_parent;
            Coroutine m_coroutineConvertFrame;

            public WebCamTexture webCamTexture => m_webcamTexture;

            public VideoStreamSourceWebCam(VideoStreamSender parent) : base(parent)
            {
                int deviceIndex = parent.m_WebCamDeviceIndex;
                if (deviceIndex < 0 || WebCamTexture.devices.Length <= deviceIndex)
                    throw new ArgumentOutOfRangeException("deviceIndex", deviceIndex, "The deviceIndex is out of range");
                m_parent = parent;
                m_deviceIndex = deviceIndex;
                m_frameRate = parent.m_FrameRate;
                m_autoRequestUserAuthorization = parent.m_AutoRequestUserAuthorization;
            }

            public override WaitForCreateTrack CreateTrack()
            {
                var instruction = new WaitForCreateTrack();
                m_parent.StartCoroutine(CreateTrackCoroutine(instruction));
                return instruction;
            }

            IEnumerator CreateTrackCoroutine(WaitForCreateTrack instruction)
            {
                if (m_autoRequestUserAuthorization)
                {
                    AsyncOperation op = Application.RequestUserAuthorization(UserAuthorization.WebCam);
                    yield return op;
                }
                if (!Application.HasUserAuthorization(UserAuthorization.WebCam))
                    throw new InvalidOperationException("Call Application.RequestUserAuthorization before creating track with WebCam.");

                WebCamDevice userCameraDevice = WebCamTexture.devices[m_deviceIndex];
                m_webcamTexture = new WebCamTexture(userCameraDevice.name, width, height, (int)m_frameRate);
                m_webcamTexture.Play();
                yield return new WaitUntil(() => m_webcamTexture.didUpdateThisFrame);
                if (m_webcamTexture.width != width || m_webcamTexture.height != height)
                {
                    Destroy(m_webcamTexture);
                    m_webcamTexture = null;
                    throw new InvalidOperationException($"The device doesn't support the resolution. {width} x {height}");
                }

                /// Convert texture if the graphicsFormat is not supported.
                /// Since Unity 2022.1, WebCamTexture.graphicsFormat returns R8G8B8A8_SRGB on Android Vulkan.
                /// WebRTC doesn't support the graphics format when using Vulkan, and throw exception.
                var supportedFormat = WebRTC.WebRTC.GetSupportedGraphicsFormat(SystemInfo.graphicsDeviceType);
                if (m_webcamTexture.graphicsFormat != supportedFormat)
                {
                    m_webcamCopyTexture = new Texture2D(width, height, supportedFormat, TextureCreationFlags.None);
                    instruction.Done(new VideoStreamTrack(m_webcamCopyTexture));
                    m_coroutineConvertFrame = m_parent.StartCoroutine(ConvertFrame());
                }
                else
                {
                    instruction.Done(new VideoStreamTrack(m_webcamTexture));
                }
            }

            IEnumerator ConvertFrame()
            {
                while (true)
                {
                    yield return new WaitForEndOfFrame();
                    Graphics.ConvertTexture(m_webcamTexture, m_webcamCopyTexture);
                }
            }

            public override void Dispose()
            {
                if (m_coroutineConvertFrame != null)
                {
                    m_parent.StopCoroutine(m_coroutineConvertFrame);
                    m_parent = null;
                    Destroy(m_webcamCopyTexture);
                    m_webcamCopyTexture = null;
                }
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

        internal IEnumerable<RTCRtpCodecCapability> SelectCodecCapabilities(IEnumerable<VideoCodecInfo> codecs)
        {
            return RTCRtpSender.GetCapabilities(TrackKind.Video).SelectCodecCapabilities(codecs);
        }
    }
}
