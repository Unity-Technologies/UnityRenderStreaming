using System.IO;
using UnityEditorInternal;
using UnityEngine;

namespace Unity.RenderStreaming.Editor
{
    internal class RenderStreamingProjectSettings : ScriptableObject
    {
        const string filePath = "ProjectSettings/RenderStreamingProjectSettings.asset";

        [SerializeField] private bool m_WizardPopupAtStart = true;
        [SerializeField] private bool m_WizardPopupAlreadyShownOnce = false;

        public static bool wizardIsStartPopup
        {
            get => instance.m_WizardPopupAtStart;
            set
            {
                instance.m_WizardPopupAtStart = value;
                Save();
            }
        }

        public static bool wizardPopupAlreadyShownOnce
        {
            get => instance.m_WizardPopupAlreadyShownOnce;
            set
            {
                instance.m_WizardPopupAlreadyShownOnce = value;
                Save();
            }
        }

        static RenderStreamingProjectSettings s_Instance;
        static RenderStreamingProjectSettings instance => s_Instance ? s_Instance : CreateOrLoad();

        RenderStreamingProjectSettings()
        {
            s_Instance = this;
        }

        static RenderStreamingProjectSettings CreateOrLoad()
        {
            // Object loaded and created from this method.
            // So RenderStreamingProjectSettings constructor is called implicitly.
            // If RenderStreamingProjectSettings could be loaded, then s_Instance is assigned the loaded instance.
            InternalEditorUtility.LoadSerializedFileAndForget(filePath);

            if (s_Instance == null)
            {
                var created = CreateInstance<RenderStreamingProjectSettings>();
                created.hideFlags = HideFlags.HideAndDontSave;
            }

            System.Diagnostics.Debug.Assert(s_Instance != null);
            return s_Instance;
        }

        static void Save()
        {
            if (s_Instance == null)
            {
                RenderStreaming.Logger.Log("Cannot save ScriptableSingleton: no instance!");
                return;
            }

            string folderPath = Path.GetDirectoryName(filePath);
            if (!Directory.Exists(folderPath))
            {
                Directory.CreateDirectory(folderPath);
            }

            InternalEditorUtility.SaveToSerializedFileAndForget(new Object[] { s_Instance }, filePath,
                allowTextSerialization: true);
        }
    }
}
