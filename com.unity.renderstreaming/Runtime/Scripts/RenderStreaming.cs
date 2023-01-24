using Unity.WebRTC;
using UnityEngine;
using Object = UnityEngine.Object;

#if UNITY_EDITOR
using UnityEditor;
#endif

namespace Unity.RenderStreaming
{
#if UNITY_EDITOR
    [InitializeOnLoad]
#endif
    public static class RenderStreaming
    {
        internal static RenderStreamingSettings s_settings;
        internal static GameObject s_automaticStreamingObject;

        public static bool AutomaticStreaming
        {
            get => s_settings.automaticStreaming;
            set
            {
                s_settings.automaticStreaming = value;
                ApplySettings();
            }
        }

        public static T GetSignalingSettings<T>() where T : SignalingSettings
        {
            return s_settings.signalingSettings as T;
        }

        static RenderStreaming()
        {
            // todo: load from assets
            var settings = ScriptableObject.CreateInstance<RenderStreamingSettings>();
            settings.automaticStreaming = false;
            var signalingSettings = new WebSocketSignalingSettings
            {
                urlSignaling = "ws://127.0.0.1:80",
                iceServers = new RTCIceServer[]
                {
                    new RTCIceServer() {urls = new string[] {"stun:stun.l.google.com:19302"}}
                }
            };
            settings.signalingSettings = signalingSettings;
            s_settings = settings;
        }

        [RuntimeInitializeOnLoadMethod(RuntimeInitializeLoadType.BeforeSceneLoad)]
        private static void RunInitialize()
        {
            if (AutomaticStreaming)
            {
                CreateAutomaticStreaming();
            }
        }

        private static void ApplySettings()
        {
            if (!Application.isPlaying)
            {
                return;
            }

            if (s_settings.automaticStreaming && s_automaticStreamingObject == null)
            {
                CreateAutomaticStreaming();
            }

            if (!s_settings.automaticStreaming)
            {
                CleanUpAutomaticStreaming();
            }
        }

        private static void CreateAutomaticStreaming()
        {
            if (s_automaticStreamingObject != null)
            {
                Object.DestroyImmediate(s_automaticStreamingObject);
            }

            s_automaticStreamingObject = new GameObject("AutomaticStreaming");
            s_automaticStreamingObject.AddComponent<AutomaticStreaming>();
            Object.DontDestroyOnLoad(s_automaticStreamingObject);
        }

        private static void CleanUpAutomaticStreaming()
        {
            Object.DestroyImmediate(s_automaticStreamingObject);
            s_automaticStreamingObject = null;
        }
    }
}
