using System;
using System.Linq;
using System.Collections.Generic;
using System.Threading;
using UnityEngine;
using Unity.WebRTC;
using Unity.RenderStreaming.Signaling;

#if UNITY_EDITOR
using UnityEditor;
#endif

namespace Unity.RenderStreaming
{
    [AddComponentMenu("Render Streaming/Render Streaming Handler")]
    public sealed class RenderStreamingHandler : MonoBehaviour
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

        [SerializeField, Tooltip("List of handlers of signaling process.")]
        private List<SignalingHandlerBase> handlers = new List<SignalingHandlerBase>();

        /// <summary>
        ///
        /// </summary>
        [SerializeField, Tooltip("Automatically started when called Awake method.")]
        public bool runOnAwake = true;
#pragma warning restore 0649

        private RenderStreamingInternal m_instance;
        private SignalingEventProvider m_provider;
        private bool m_running;

        static Type GetType(string typeName)
        {
            var type = Type.GetType(typeName);
            if (type != null) return type;
            foreach (var assembly in System.AppDomain.CurrentDomain.GetAssemblies())
            {
                type = assembly.GetType(typeName);
                if (type != null) return type;
            }
            return null;
        }

        static ISignaling CreateSignaling(string type, string url, float interval, SynchronizationContext context)
        {
            Type _type = GetType(type);
            if (_type == null)
            {
                throw new ArgumentException($"Signaling type is undefined. {type}");
            }
            object[] args = { url, interval, context };
            return (ISignaling)Activator.CreateInstance(_type, args);
        }

        /// <summary>
        ///
        /// </summary>
        /// <param name="settings"></param>
        /// <exception cref="InvalidOperationException"></exception>
        public void SetSignalingSettings(SignalingSettings settings)
        {
            if (m_running)
            {
                throw new InvalidOperationException("The Signaling process has already started.");
            }

            urlSignaling = settings.urlSignaling;
            signalingType = settings.signalingClass.FullName;
            iceServers = settings.iceServers;
        }

        /// <summary>
        ///
        /// </summary>
        /// <param name="handlerBase"></param>
        public void AddSignalingHandler(SignalingHandlerBase handlerBase)
        {
            if (handlers.Contains(handlerBase))
            {
                return;
            }
            handlers.Add(handlerBase);

            if (!m_running)
            {
                return;
            }
            handlerBase.SetHandler(m_instance);
            m_provider.Subscribe(handlerBase);
        }

        /// <summary>
        ///
        /// </summary>
        /// <param name="handlerBase"></param>
        public void RemoveSignalingHandler(SignalingHandlerBase handlerBase)
        {
            handlers.Remove(handlerBase);

            if (!m_running)
            {
                return;
            }
            handlerBase.SetHandler(null);
            m_provider.Unsubscribe(handlerBase);
        }

        /// <summary>
        ///
        /// </summary>
        /// <param name="signaling"></param>
        /// <param name="handlers"></param>
        public void Run(
            ISignaling signaling = null,
            SignalingHandlerBase[] handlers = null)
        {
            _Run(null, signaling, handlers);
        }

        /// <summary>
        ///
        /// </summary>
        /// <param name="conf"></param>
        /// <param name="signaling"></param>
        /// <param name="handlers"></param>
        /// <remarks> To use this method, Need to depend WebRTC package </remarks>
        public void Run(
            RTCConfiguration conf,
            ISignaling signaling = null,
            SignalingHandlerBase[] handlers = null
            )
        {
            _Run(conf, signaling, handlers);
        }

        /// <summary>
        ///
        /// </summary>
        /// <param name="conf"></param>
        /// <param name="signaling"></param>
        /// <param name="handlers"></param>
        private void _Run(
            RTCConfiguration? conf = null,
            ISignaling signaling = null,
            SignalingHandlerBase[] handlers = null
            )
        {
            RTCConfiguration _conf =
                conf.GetValueOrDefault(new RTCConfiguration { iceServers = iceServers });

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
                signaling = _signaling,
                startCoroutine = StartCoroutine,
                stopCoroutine = StopCoroutine,
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

        /// <summary>
        ///
        /// </summary>
        public void Stop()
        {
            m_instance?.Dispose();
            m_instance = null;
            m_running = false;
        }

        void Awake()
        {
            if (!runOnAwake || m_running　|| handlers.Count == 0)
                return;

            RTCConfiguration conf = new RTCConfiguration { iceServers = iceServers };
            ISignaling signaling = CreateSignaling(
                signalingType, urlSignaling, interval, SynchronizationContext.Current);
            _Run(conf, signaling, handlers.ToArray());
        }

        void OnDestroy()
        {
            Stop();
        }
    }
}
