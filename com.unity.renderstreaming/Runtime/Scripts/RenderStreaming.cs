using System;
using System.Linq;
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
        internal const string EditorBuildSettingsConfigKey = "com.unity.renderstreaming.settings";
        internal const string DefaultRenderStreamingSettingsPath =
            "Packages/com.unity.renderstreaming/Runtime/RenderStreamingSettings.asset";

        private static RenderStreamingSettings s_settings;
        private static GameObject s_automaticStreamingObject;

        private static bool m_running;

        internal static RenderStreamingSettings Settings
        {
            get => s_settings;
            set
            {
                if (value == null)
                    throw new ArgumentNullException(nameof(value));

                if (s_settings == value)
                    return;

                // In the editor, we keep track of the settings asset through EditorBuildSettings.
#if UNITY_EDITOR
                if (!string.IsNullOrEmpty(AssetDatabase.GetAssetPath(value)))
                {
                    EditorBuildSettings.AddConfigObject(EditorBuildSettingsConfigKey, value, true);
                }
#endif

                if (m_running && s_settings.signalingSettings != value.signalingSettings)
                {
                    Debug.LogWarning("Signaling settings doesn't change on already started signaling instance.");
                }

                s_settings = value;
                ApplySettings();
            }
        }

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
            if (m_running)
            {
                throw new InvalidOperationException("Signaling settings can't overwrite on playing.");
            }

            s_settings.signalingSettings = settings;
            ApplySettings();
        }
#endif

        static RenderStreaming()
        {
#if UNITY_EDITOR
            InitializeInEditor();
#else
            m_running = true;
#endif
        }

#if UNITY_EDITOR
        private static void InitializeInEditor()
        {
            if (EditorBuildSettings.TryGetConfigObject(EditorBuildSettingsConfigKey, out RenderStreamingSettings settingsAsset))
            {
                s_settings = settingsAsset;
            }
            else
            {
                s_settings = AssetDatabase.LoadAssetAtPath<RenderStreamingSettings>(DefaultRenderStreamingSettingsPath);
            }

            EditorApplication.playModeStateChanged += change =>
            {
                m_running = change == PlayModeStateChange.EnteredPlayMode;
            };

            EditorApplication.projectChanged += () =>
            {
                if (!EditorBuildSettings.TryGetConfigObject(EditorBuildSettingsConfigKey, out RenderStreamingSettings _))
                {
                    Settings = AssetDatabase.LoadAssetAtPath<RenderStreamingSettings>(DefaultRenderStreamingSettingsPath);
                }
            };
        }
#endif

        [RuntimeInitializeOnLoadMethod(RuntimeInitializeLoadType.SubsystemRegistration)]
        private static void LoadSettings()
        {
            if (s_settings == null)
            {
                s_settings = Resources.FindObjectsOfTypeAll<RenderStreamingSettings>().FirstOrDefault() ??
                             ScriptableObject.CreateInstance<RenderStreamingSettings>();
            }
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
            if (!m_running)
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
