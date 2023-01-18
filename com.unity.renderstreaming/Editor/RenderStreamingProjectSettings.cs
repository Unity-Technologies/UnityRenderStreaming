using System.IO;
using UnityEditorInternal;
using UnityEngine;

namespace Editor
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
                Debug.Log("Cannot save ScriptableSingleton: no instance!");
                return;
            }

            string folderPath = Path.GetDirectoryName(filePath);
            if (!Directory.Exists(folderPath))
            {
                Directory.CreateDirectory(folderPath);
            }

            InternalEditorUtility.SaveToSerializedFileAndForget(new Object[] {s_Instance}, filePath,
                allowTextSerialization: true);
        }
    }
}
