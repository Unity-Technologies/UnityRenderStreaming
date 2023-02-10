using System;
using System.IO;
using System.Linq;
using Unity.RenderStreaming.Editor;
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
            cacheGuid = AssetDatabase.FindAssets($"t:Script {nameof(SampleSetup)}")[0];

            Load();
            if (s_Settings.dialogAlreadyShowOnStartup || !RenderStreaming.AutomaticStreaming)
            {
                return;
            }

            const string dialogText =
                "It is recommended to turn off AutomaticStreaming in the scenes included in the sample. Do you want to change the config assets for Sample?";
            if (EditorUtility.DisplayDialog("Warning", dialogText, "Change Settings", "Ignore"))
            {
                var guids = AssetDatabase.FindAssets("t:RenderStreamingSettings");
                var path = guids.Select(AssetDatabase.GUIDToAssetPath).First(x => x.EndsWith("RenderStreamingSample.asset"));
                var asset = AssetDatabase.LoadAssetAtPath<RenderStreamingSettings>(path);
                if (asset != null)
                {
                    RenderStreamingEditor.SetRenderStreamingSettings(asset);
                }
                else
                {
                    Debug.LogError("RenderStreamingSample.asset not found.");
                }
            }

            s_Settings.dialogAlreadyShowOnStartup = true;
            Save();
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
