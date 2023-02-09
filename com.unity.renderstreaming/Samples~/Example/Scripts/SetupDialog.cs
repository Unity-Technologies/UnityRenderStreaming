#if UNITY_EDITOR
using System;
using System.IO;
using UnityEditor;
using UnityEngine;


namespace Unity.RenderStreaming.Samples
{
    [InitializeOnLoad]
    internal class SetupDialog
    {
        private const string kSavePath = "Library/RenderStreamingSampleSettings.json";

        static SetupDialog()
        {
            Load();
            if (s_Settings.dialogAlreadyShowOnStartup)
            {
                return;
            }

            s_Settings.dialogAlreadyShowOnStartup = true;
            Save();
        }

        [Serializable]
        internal struct SerializedState
        {
            public bool dialogAlreadyShowOnStartup;
        }

        internal static SerializedState s_Settings;

        internal static void Load()
        {
            s_Settings = new SerializedState();
            if (!File.Exists(kSavePath))
                return;

            try
            {
                var json = File.ReadAllText(kSavePath);
                s_Settings = JsonUtility.FromJson<SerializedState>(json);
            }
            catch
            {
                s_Settings = new SerializedState();
            }
        }

        internal static void Save()
        {
            var json = JsonUtility.ToJson(s_Settings, prettyPrint: true);
            File.WriteAllText(kSavePath, json);
        }
    }
}
#endif
