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
        public const string EditorBuildSettingsConfigKey = "com.unity.renderstreaming.settings";
        public const string DefaultRenderStreamingSettingsPath = "Packages/com.unity.renderstreaming/Runtime/RenderStreamingSettings.asset";
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

#if UNITY_EDITOR
        public static void SetSignalingSettings(SignalingSettings settings)
        {
            if (Application.isPlaying)
            {
                throw new InvalidOperationException("Signaling settings can't overwrite on playing.");
            }
            s_settings.signalingSettings = settings;
            ApplySettings();
            Debug.Log("");
        }
#endif

        static RenderStreaming()
        {
#if UNITY_EDITOR
            if (EditorBuildSettings.TryGetConfigObject(EditorBuildSettingsConfigKey, out RenderStreamingSettings settingsAsset))
            {
                s_settings = settingsAsset;
            }
            else
            {
                s_settings = AssetDatabase.LoadAssetAtPath<RenderStreamingSettings>(DefaultRenderStreamingSettingsPath);
            }
#endif
        }

        [RuntimeInitializeOnLoadMethod(loadType: RuntimeInitializeLoadType.SubsystemRegistration)]
        private static void RunInitialize()
        {
            if (s_settings == null)
            {
                s_settings = Resources.FindObjectsOfTypeAll<RenderStreamingSettings>().FirstOrDefault() ??
                             ScriptableObject.CreateInstance<RenderStreamingSettings>();
            }

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
