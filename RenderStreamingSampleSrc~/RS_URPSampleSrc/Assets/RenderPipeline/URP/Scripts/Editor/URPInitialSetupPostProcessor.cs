﻿using UnityEngine;                                      //Debug
using UnityEditor;                                      //AssetPostProcessor
using UnityEngine.Rendering;                            //GraphicsSettings

public class URPInitialSetupPostProcessor : AssetPostprocessor
{
    static void OnPostprocessAllAssets(string[] importedAssets, string[] deletedAssets,
        string[] movedAssets, string[] movedFromAssetPaths)
    {
        if (null != GraphicsSettings.renderPipelineAsset) {
            return;
        }
        
        int numImportedAssets = importedAssets.Length;
        for (int i=0;i< numImportedAssets; ++i) {
            string curAssetPath = importedAssets[i];

            if (curAssetPath == "Assets/RenderPipeline/URP/UniversalRenderPipelineAsset.asset") {
                UnityEngine.Rendering.Universal.UniversalRenderPipelineAsset pipelineAsset =
                    AssetDatabase.LoadAssetAtPath<UnityEngine.Rendering.Universal.UniversalRenderPipelineAsset>(curAssetPath);
                GraphicsSettings.renderPipelineAsset = pipelineAsset;
                PlayerSettings.colorSpace = ColorSpace.Linear;
            }
        }
    }
}
