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
    [AddComponentMenu("Render Streaming/Signaling Manager")]
    public sealed class SignalingManager : MonoBehaviour
    {
#pragma warning disable 0649
        // ToDo: Create component UI on URS-553
        [SerializeReference, SignalingSettings]
        private SignalingSettings signalingSettings = new WebSocketSignalingSettings
        (
            url: "ws://127.0.0.1:80",
            iceServers: new[]
            {
                new IceServer(urls: new[] {"stun:stun.l.google.com:19302"})
            }
        );

        [SerializeField, Tooltip("List of handlers of signaling process.")]
        private List<SignalingHandlerBase> handlers = new List<SignalingHandlerBase>();

        /// <summary>
        ///
        /// </summary>
        [SerializeField, Tooltip("Automatically started when called Awake method.")]
        public bool runOnAwake = true;
#pragma warning restore 0649

        private SignalingManagerInternal m_instance;
        private SignalingEventProvider m_provider;
        private bool m_running;

        public bool Running => m_running;

        static ISignaling CreateSignaling(SignalingSettings settings, SynchronizationContext context)
        {
            if (settings.signalingClass == null)
            {
                throw new ArgumentException($"Signaling type is undefined. {settings.signalingClass}");
            }
            object[] args = { settings, context };
            return (ISignaling)Activator.CreateInstance(settings.signalingClass, args);
        }

        /// <summary>
        ///
        /// </summary>
        /// <param name="settings"></param>
        /// <exception cref="InvalidOperationException"></exception>
        public void SetSignalingSettings(SignalingSettings settings)
        {
            if (m_running)
                throw new InvalidOperationException("The Signaling process has already started.");

            if (settings == null)
                throw new ArgumentNullException("settings");

            signalingSettings = settings;
        }

        public SignalingSettings GetSignalingSettings()
        {
            return signalingSettings;
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
            RTCIceServer[] iceServers = signalingSettings.iceServers.OfType<RTCIceServer>().ToArray();
            RTCConfiguration _conf =
                conf.GetValueOrDefault(new RTCConfiguration { iceServers = iceServers });

            ISignaling _signaling = signaling ?? CreateSignaling(signalingSettings, SynchronizationContext.Current);
            RenderStreamingDependencies dependencies = new RenderStreamingDependencies
            {
                config = _conf,
                signaling = _signaling,
                startCoroutine = StartCoroutine,
                stopCoroutine = StopCoroutine,
                resentOfferInterval = 5.0f,
            };
            var _handlers = (handlers ?? this.handlers.AsEnumerable()).Where(_ => _ != null);
            if (_handlers.Count() == 0)
                throw new InvalidOperationException("Handler list is empty.");

            m_instance = new SignalingManagerInternal(ref dependencies);
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
            if (!runOnAwake || m_running || handlers.Count == 0)
                return;

            RTCIceServer[] iceServers = signalingSettings.iceServers.Cast<RTCIceServer>().ToArray();
            RTCConfiguration conf = new RTCConfiguration { iceServers = iceServers };
            ISignaling signaling = CreateSignaling(signalingSettings, SynchronizationContext.Current);
            _Run(conf, signaling, handlers.ToArray());
        }

        void OnDestroy()
        {
            Stop();
        }
    }
}
