using System;
using Unity.WebRTC;
using UnityEngine;

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
        private static RenderStreamingSettings s_Settings;

        public static RenderStreamingSettings Settings
        {
            get => s_Settings;
            set
            {
                if (value == null)
                    throw new ArgumentNullException(nameof(value));

                if (s_Settings == value)
                    return;

                s_Settings = value;
            }
        }

        public static bool AutomaticStreaming
        {
            get => s_Settings.automaticStreaming;
            set => s_Settings.automaticStreaming = value;
        }

        public static T GetSignalingSettings<T>() where T : SignalingSettings
        {
            return s_Settings.signalingSettings as T;
        }

        static RenderStreaming()
        {
            // todo: load from assets
            var settings = ScriptableObject.CreateInstance<RenderStreamingSettings>();
            settings.automaticStreaming = true;
            var signalingSettings = new WebSocketSignalingSettings();
            signalingSettings.urlSignaling = "127.0.0.1:80";
            signalingSettings.iceServers = new RTCIceServer[]
            {
                new RTCIceServer() {urls = new string[] {"stun:stun.l.google.com:19302"}}
            };
            settings.signalingSettings = signalingSettings;
            s_Settings = settings;
        }
    }
}
