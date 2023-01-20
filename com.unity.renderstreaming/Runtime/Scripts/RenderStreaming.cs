using System;
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
        private static RenderStreamingSettings s_settings;
        private static GameObject s_automaticStreamingObject;

        public static RenderStreamingSettings Settings
        {
            get => s_settings;
            set
            {
                if (value == null)
                    throw new ArgumentNullException(nameof(value));

                if (s_settings == value)
                    return;

                s_settings = value;
                ApplySettings();
            }
        }

        public static bool AutomaticStreaming
        {
            get => s_settings.AutomaticStreaming;
            set => s_settings.AutomaticStreaming = value;
        }

        public static T GetSignalingSettings<T>() where T : SignalingSettings
        {
            return s_settings.SignalingSettings as T;
        }

        static RenderStreaming()
        {
            // todo: load from assets
            var settings = ScriptableObject.CreateInstance<RenderStreamingSettings>();
            settings.AutomaticStreaming = false;
            var signalingSettings = new WebSocketSignalingSettings
            {
                urlSignaling = "ws://127.0.0.1:80",
                iceServers = new RTCIceServer[]
                {
                    new RTCIceServer() {urls = new string[] {"stun:stun.l.google.com:19302"}}
                }
            };
            settings.SignalingSettings = signalingSettings;
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

        internal static void ApplySettings()
        {
            if (!Application.isPlaying)
            {
                return;
            }

            if (s_settings.AutomaticStreaming && s_automaticStreamingObject == null)
            {
                CreateAutomaticStreaming();
            }

            if (!s_settings.AutomaticStreaming)
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
