#if UNITY_EDITOR
using System;
using System.IO;
using UnityEditor;
using UnityEngine;


namespace Unity.RenderStreaming.Samples
{
    [InitializeOnLoad]
    class SampleSetup
    {
        private const string kSavePath = "Library/RenderStreamingSampleSettings.json";
        private static string cacheGuid = "";

        class DeleteSampleSettings : UnityEditor.AssetModificationProcessor
        {
            // When SampleSetup script is deleted, also delete RenderStreamingSampleSettings.json.
            private static AssetDeleteResult OnWillDeleteAsset(string assetPath, RemoveAssetOptions options)
            {
                var existPath = AssetDatabase.GUIDToAssetPath(cacheGuid);
                if (existPath.StartsWith(assetPath))
                {
                    File.Delete(kSavePath);
                }

                return AssetDeleteResult.DidNotDelete;
            }
        }

        static SampleSetup()
        {
            if (Application.isBatchMode)
            {
                return;
            }

            cacheGuid = AssetDatabase.FindAssets($"t:Script {nameof(SampleSetup)}")[0];

            Load();
            if (s_Settings.dialogAlreadyShowOnStartup || !RenderStreaming.AutomaticStreaming)
            {
                return;
            }

            const string dialogText =
                "It is recommended to use the sample scene with Automatic Streaming turn off. Please change Render Streaming Settings on Project settings.";
            if (EditorUtility.DisplayDialog("Warning", dialogText, "Open Project settings.", "Ignore"))
            {
                //We need to wait at least one frame or the popup will not show up
                frameToWait = 10;
                EditorApplication.update += OpenSettingsDelayed;
            }

            s_Settings.dialogAlreadyShowOnStartup = true;
            Save();
        }

        private static int frameToWait;

        private static void OpenSettingsDelayed()
        {
            if (frameToWait > 0)
            {
                --frameToWait;
            }
            else
            {
                EditorApplication.update -= OpenSettingsDelayed;
                SettingsService.OpenProjectSettings("Project/Render Streaming");
            }
        }

        [Serializable]
        private struct SerializedState
        {
            public bool dialogAlreadyShowOnStartup;
        }

        private static SerializedState s_Settings;

        private static void Load()
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

        private static void Save()
        {
            var json = JsonUtility.ToJson(s_Settings, prettyPrint: true);
            File.WriteAllText(kSavePath, json);
        }
    }
}
#endif
