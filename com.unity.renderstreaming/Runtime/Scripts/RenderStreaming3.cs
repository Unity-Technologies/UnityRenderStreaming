using System;
using System.Collections.Generic;
using System.Threading;
using UnityEngine;
using Unity.WebRTC;
using Unity.RenderStreaming.Signaling;
using UnityEngine.InputSystem.EnhancedTouch;

namespace Unity.RenderStreaming
{
    public sealed class RenderStreaming3 : MonoBehaviour
    {
#pragma warning disable 0649
        [SerializeField, Tooltip("Signaling server url")]
        private string urlSignaling = "http://localhost";

        [SerializeField, Tooltip("Type of signaling server")]
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

#pragma warning restore 0649

        private SynchronizationContext m_mainThreadContext;
        private RenderStreamingInternal instance;
        private SignalingEventProvider provider;

        public void Awake()
        {
            RTCConfiguration conf = new RTCConfiguration {iceServers = iceServers};
            var encoderType = hardwareEncoderSupport ? EncoderType.Hardware : EncoderType.Software;
            Type _type = Type.GetType(signalingType);
            if (_type == null)
            {
                throw new ArgumentException($"Signaling type is undefined. {signalingType}");
            }
            m_mainThreadContext = SynchronizationContext.Current;
            object[] args = { urlSignaling, interval, m_mainThreadContext };
            RenderStreamingDependencies dependencies = new RenderStreamingDependencies
            {
                config = conf,
                encoderType = encoderType,
                signaling = (ISignaling)Activator.CreateInstance(_type, args),
                startCoroutine = StartCoroutine
            };
            instance = new RenderStreamingInternal(ref dependencies);
            provider = new SignalingEventProvider(instance);

            foreach (var source in handlers)
            {
                source.SetHandler(instance);
                provider.Subscribe(source);
            }
            EnhancedTouchSupport.Enable();
        }

        public void OnDestroy()
        {
            instance.Dispose();

            EnhancedTouchSupport.Disable();
            RemoteInputReceiver.Dispose();
            m_mainThreadContext = null;
        }
    }
}
