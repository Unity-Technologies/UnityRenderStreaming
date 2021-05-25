using System;
using System.Collections.Generic;
using System.Threading;
using UnityEngine;
using Unity.WebRTC;
using Unity.RenderStreaming.Signaling;
using System.IO;

namespace Unity.RenderStreaming
{
    public sealed class RenderStreaming : MonoBehaviour
    {
        public string url
        {
            set
            {
                Debug.LogFormat("Token {0}. Method {1}. URL = {2}", "JSHARIFI TOKEN", new System.Diagnostics.StackFrame().GetMethod(), value);
                
                urlSignaling = value;
            }
        }

#pragma warning disable 0649
        [SerializeField, Tooltip("Signaling server url")]
        private string urlSignaling = "http://localhost";

        [SerializeField, Tooltip("Type of signaling")]
        private string signalingType = typeof(HttpSignaling).FullName;

        [SerializeField, Tooltip("Array to set your own STUN/TURN servers")]
        private RTCIceServer[] iceServers = new RTCIceServer[]
        {
            new RTCIceServer() {urls = new string[] {"stun:stun.l.google.com:19302"}}
        };

        [SerializeField, Tooltip("Time interval for polling from signaling server")]
        private float interval = 5.0f;

        [SerializeField, Tooltip("Enable or disable hardware encoder")]
        private bool hardwareEncoderSupport = true;

        [SerializeField]
        private List<SignalingHandlerBase> handlers = new List<SignalingHandlerBase>();

        [SerializeField]
        public bool runOnAwake = true;
#pragma warning restore 0649

        private RenderStreamingInternal m_instance;
        private SignalingEventProvider m_provider;
        private bool m_running;

        static ISignaling CreateSignaling(
            string type, string url, float interval, SynchronizationContext context)
        {
            Debug.LogFormat("Token {0}. Method {1}. urlSignaling is {2}", "JSHARIFI TOKEN", new System.Diagnostics.StackFrame().GetMethod(), url);

            Type _type = Type.GetType(type);
            if (_type == null)
            {
                throw new ArgumentException($"Signaling type is undefined. {type}");
            }
            object[] args = { url, interval, context };
            return (ISignaling)Activator.CreateInstance(_type, args);
        }

        [System.Serializable]
        struct dataIn
        {
            public string uriHost;

            public override string ToString()
            {
                return JsonUtility.ToJson(this, true);
            }
        }

        dataIn mConfig;

        public void UpdateConfig()
        {
            string path = Path.Combine(Application.streamingAssetsPath, "config.json");
            if (Application.platform == RuntimePlatform.Android) path = Path.Combine(Application.persistentDataPath, "config.json");

            if (File.Exists(path))
            {
                string text = File.ReadAllText(path);

                if (string.IsNullOrEmpty(text))
                {
                    return;
                }
                
                mConfig = JsonUtility.FromJson<dataIn>(text);
                
                if (!string.IsNullOrEmpty(mConfig.uriHost))
                {
                    Debug.LogFormat("update urlSignaling to {0}", mConfig.uriHost);
                    urlSignaling = mConfig.uriHost;
                }
                else
                {
                    Debug.LogFormat("No param for uriHost");
                }
            }
            else
            {
                Debug.LogFormat("No file at {0}", path);
            }
        }

        void Awake()
        {
            if (!runOnAwake || m_running)
                return;

            UpdateConfig();

            Debug.LogFormat("Token {0}. Method {1}. urlSignaling is {2}", "JSHARIFI TOKEN", new System.Diagnostics.StackFrame().GetMethod(), urlSignaling);

            RTCConfiguration conf = new RTCConfiguration {iceServers = iceServers};
            ISignaling signaling = CreateSignaling(
                signalingType, urlSignaling, interval, SynchronizationContext.Current);
            Run(conf, hardwareEncoderSupport, signaling, handlers.ToArray());
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="conf"></param>
        /// <param name="hardwareEncoder"></param>
        /// <param name="signaling"></param>
        /// <param name="handlers"></param>
        public void Run(
            RTCConfiguration? conf = null,
            bool? hardwareEncoder = null,
            ISignaling signaling = null,
            SignalingHandlerBase[] handlers = null
            )
        {
            RTCConfiguration _conf =
                conf.GetValueOrDefault(new RTCConfiguration { iceServers = iceServers });
            bool _hardwareEncoder =
                hardwareEncoder.GetValueOrDefault(hardwareEncoderSupport);
            var encoderType = _hardwareEncoder ? EncoderType.Hardware : EncoderType.Software;
            ISignaling _signaling = signaling ?? CreateSignaling(
                signalingType, urlSignaling, interval, SynchronizationContext.Current);
            RenderStreamingDependencies dependencies = new RenderStreamingDependencies
            {
                config = _conf,
                encoderType = encoderType,
                signaling = _signaling,
                startCoroutine = StartCoroutine
            };
            m_instance = new RenderStreamingInternal(ref dependencies);
            m_provider = new SignalingEventProvider(m_instance);

            SignalingHandlerBase[] _handlers = handlers ?? this.handlers.ToArray();

            foreach (var source in _handlers)
            {
                source.SetHandler(m_instance);
                m_provider.Subscribe(source);
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
            //EnhancedTouchSupport.Disable();
            //RemoteInputReceiver.Dispose();
        }
    }
}
