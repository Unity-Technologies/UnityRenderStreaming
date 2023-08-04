using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;
using System.Threading;
using Unity.RenderStreaming.Signaling;
using Unity.WebRTC;
using UnityEngine;

#if UNITY_EDITOR
using UnityEditor;
#endif

namespace Unity.RenderStreaming
{
    [AddComponentMenu("Render Streaming/Signaling Manager")]
    public sealed class SignalingManager : MonoBehaviour
    {
        internal const string UseDefaultPropertyName = nameof(m_useDefault);
        internal const string SignalingSettingsObjectPropertyName = nameof(signalingSettingsObject);
        internal const string SignalingSettingsPropertyName = nameof(signalingSettings);
        internal const string HandlersPropertyName = nameof(handlers);
        internal const string RunOnAwakePropertyName = nameof(runOnAwake);
        internal const string EvaluateCommandlineArgumentsPropertyName = nameof(evaluateCommandlineArguments);

#pragma warning disable 0649
        [SerializeField, Tooltip("Use settings in Project Settings Window.")]
        private bool m_useDefault = true;

        [SerializeField]
        internal SignalingSettingsObject signalingSettingsObject;

        [SerializeReference, SignalingSettings]
        private SignalingSettings signalingSettings = new WebSocketSignalingSettings();

        [SerializeField, Tooltip("List of handlers of signaling process.")]
        private List<SignalingHandlerBase> handlers = new List<SignalingHandlerBase>();

        /// <summary>
        ///
        /// </summary>
        [SerializeField, Tooltip("Automatically started when called Awake method.")]
        public bool runOnAwake = true;

        /// <summary>
        ///
        /// </summary>
        [SerializeField, Tooltip("Evaluate commandline arguments if launching runtime on the command line.")]
        public bool evaluateCommandlineArguments = true;

#pragma warning restore 0649

        private SignalingManagerInternal m_instance;
        private SignalingEventProvider m_provider;
        private bool m_running;

        /// <summary>
        /// 
        /// </summary>
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
        /// Use settings in Project Settings.
        /// </summary>
        public bool useDefaultSettings
        {
            get { return m_useDefault; }
            set { m_useDefault = value; }
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

        /// <summary>
        ///
        /// </summary>
        /// <returns></returns>
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

#if UNITY_EDITOR
        bool IsValidSignalingSettingsObject(SignalingSettingsObject asset)
        {
            if (asset == null)
                return false;
            if (AssetDatabase.GetAssetPath(asset).IndexOf("Assets", StringComparison.Ordinal) != 0)
                return false;
            return true;
        }
#endif

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
            var settings = m_useDefault ? RenderStreaming.GetSignalingSettings<SignalingSettings>() : signalingSettings;
#if !UNITY_EDITOR
            var arguments = Environment.GetCommandLineArgs();
            if (evaluateCommandlineArguments && arguments.Length > 1)
            {
                if (!EvaluateCommandlineArguments(ref settings, arguments))
                {
                    RenderStreaming.Logger.Log(LogType.Error, "Command line arguments are invalid.");
                }
            }
#endif
            int i = 0;
            RTCIceServer[] iceServers = new RTCIceServer[settings.iceServers.Count()];
            foreach (var iceServer in settings.iceServers)
            {
                iceServers[i] = (RTCIceServer)iceServer;
                i++;
            }
            RTCConfiguration _conf =
                conf.GetValueOrDefault(new RTCConfiguration { iceServers = iceServers });

            ISignaling _signaling = signaling ?? CreateSignaling(settings, SynchronizationContext.Current);
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

        internal static bool EvaluateCommandlineArguments(ref SignalingSettings settings, string[] arguments)
        {
            if (!CommandLineParser.TryParse(arguments))
                return false;

            string signalingTypeName = null;
            if (CommandLineParser.SignalingType.Value != null)
            {
                signalingTypeName = CommandLineParser.SignalingType;
            }
            else if (CommandLineParser.ImportJson.Value != null)
            {
                signalingTypeName = CommandLineParser.ImportJson.Value.Value.signalingType;
            }
            if (signalingTypeName != null)
            {
                Type[] types = RuntimeTypeCache<SignalingSettings>.GetTypesDerivedFrom();
                Dictionary<string, Type> map =
                    types.Where(type => type.GetCustomAttribute<SignalingTypeAttribute>() != null)
                        .ToDictionary(type => type.GetCustomAttribute<SignalingTypeAttribute>().typename, type => type);

                if (map.ContainsKey(signalingTypeName))
                {
                    var type = map[signalingTypeName];
                    settings = (SignalingSettings)Activator.CreateInstance(type);
                }
            }
            return settings.ParseArguments(arguments);
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

            var settings = m_useDefault ? RenderStreaming.GetSignalingSettings<SignalingSettings>() : signalingSettings;
            int i = 0;
            RTCIceServer[] iceServers = new RTCIceServer[settings.iceServers.Count()];
            foreach (var iceServer in settings.iceServers)
            {
                iceServers[i] = (RTCIceServer)iceServer;
                i++;
            }
            RTCConfiguration conf = new RTCConfiguration { iceServers = iceServers };
            ISignaling signaling = CreateSignaling(settings, SynchronizationContext.Current);
            _Run(conf, signaling, handlers.ToArray());
        }

        void OnDestroy()
        {
            Stop();
        }
    }
}
