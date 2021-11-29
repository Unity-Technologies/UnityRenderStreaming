using System;
using System.Linq;
using System.Collections.Generic;
using System.Threading;
using UnityEngine;
using Unity.WebRTC;
using Unity.RenderStreaming.Signaling;

namespace Unity.RenderStreaming
{
    public sealed class RenderStreaming : MonoBehaviour
    {
#pragma warning disable 0649
        [SerializeField, Tooltip("Signaling server url.")]
        private string urlSignaling = "http://localhost";

        [SerializeField, Tooltip("Type of signaling.")]
        private string signalingType = typeof(HttpSignaling).FullName;

        [SerializeField, Tooltip("Array to set your own STUN/TURN servers.")]
        private RTCIceServer[] iceServers = new RTCIceServer[]
        {
            new RTCIceServer() {urls = new string[] {"stun:stun.l.google.com:19302"}}
        };

        [SerializeField, Tooltip("Time interval for polling from signaling server.")]
        private float interval = 5.0f;

        [SerializeField, Tooltip("Enable or disable hardware encoder.")]
        private bool hardwareEncoderSupport = true;

        [SerializeField, Tooltip("List of handlers of signaling process.")]
        private List<SignalingHandlerBase> handlers = new List<SignalingHandlerBase>();

        [SerializeField, Tooltip("Automatically started when called Awake method.")]
        public bool runOnAwake = true;
#pragma warning restore 0649

        private RenderStreamingInternal m_instance;
        private SignalingEventProvider m_provider;
        private bool m_running;

        static ISignaling CreateSignaling(
            string type, string url, float interval, SynchronizationContext context)
        {
            Type _type = Type.GetType(type);
            if (_type == null)
            {
                throw new ArgumentException($"Signaling type is undefined. {type}");
            }
            object[] args = { url, interval, context };
            return (ISignaling)Activator.CreateInstance(_type, args);
        }

        void Awake()
        {
            if (!runOnAwake || m_running)
                return;

            RTCConfiguration conf = new RTCConfiguration {iceServers = iceServers};
            ISignaling signaling = CreateSignaling(
                signalingType, urlSignaling, interval, SynchronizationContext.Current);
            _Run(conf, hardwareEncoderSupport, signaling, handlers.ToArray());
        }

        /// <summary>
        ///
        /// </summary>
        /// <param name="hardwareEncoder"></param>
        /// <param name="signaling"></param>
        /// <param name="handlers"></param>
        public void Run(
            bool? hardwareEncoder = null,
            ISignaling signaling = null,
            SignalingHandlerBase[] handlers = null)
        {
            _Run(null, hardwareEncoder, signaling, handlers);
        }

        /// <summary>
        ///
        /// </summary>
        /// <param name="conf"></param>
        /// <param name="hardwareEncoder"></param>
        /// <param name="signaling"></param>
        /// <param name="handlers"></param>
        /// <remarks> To use this method, Need to depend WebRTC package </remarks>
        public void Run(
            RTCConfiguration conf,
            bool? hardwareEncoder = null,
            ISignaling signaling = null,
            SignalingHandlerBase[] handlers = null
            )
        {
            _Run(conf, hardwareEncoder, signaling, handlers);
        }

        private void _Run(
            RTCConfiguration? conf = null,
            bool? hardwareEncoder = null,
            ISignaling signaling = null,
            SignalingHandlerBase[] handlers = null
            )
        {
            RTCConfiguration _conf =
                conf.GetValueOrDefault(new RTCConfiguration { iceServers = iceServers });
            if (hardwareEncoder != null)
                hardwareEncoderSupport = hardwareEncoder.Value;
            var encoderType = hardwareEncoderSupport ? EncoderType.Hardware : EncoderType.Software;

            if (signaling != null)
            {
                signalingType = signaling.GetType().FullName;

                //todo:: This property is not needed by FurioosSignaling.
                urlSignaling = signaling.Url;
                interval = signaling.Interval;
            }
            ISignaling _signaling = signaling ?? CreateSignaling(
                signalingType, urlSignaling, interval, SynchronizationContext.Current);
            RenderStreamingDependencies dependencies = new RenderStreamingDependencies
            {
                config = _conf,
                encoderType = encoderType,
                signaling = _signaling,
                startCoroutine = StartCoroutine,
                resentOfferInterval = interval,
            };
            var _handlers = (handlers ?? this.handlers.AsEnumerable()).Where(_ => _ != null);
            if (_handlers.Count() == 0)
                throw new InvalidOperationException("Handler list is empty.");

            m_instance = new RenderStreamingInternal(ref dependencies);
            m_provider = new SignalingEventProvider(m_instance);

            foreach (var handler in _handlers)
            {
                handler.SetHandler(m_instance);
                m_provider.Subscribe(handler);
            }
            m_running = true;
        }

        public void Stop()
        {
            m_instance?.Dispose();
            m_instance = null;
            m_running = false;
        }

        public void OnDestroy()
        {
            Stop();
        }
    }
}
