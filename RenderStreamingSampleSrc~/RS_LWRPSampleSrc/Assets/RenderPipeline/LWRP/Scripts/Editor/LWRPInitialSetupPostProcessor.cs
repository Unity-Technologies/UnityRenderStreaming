﻿using UnityEngine;                                      //Debug
using UnityEditor;                                      //AssetPostProcessor
using UnityEngine.Rendering;                            //GraphicsSettings
using UnityEngine.Rendering.LWRP;

public class HDRPInitialSetupPostProcessor : AssetPostprocessor
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

            if (curAssetPath == "Assets/RenderPipeline/LWRP/LightweightRenderPipelineAsset.asset") {
                LightweightRenderPipelineAsset pipelineAsset =
                    AssetDatabase.LoadAssetAtPath<LightweightRenderPipelineAsset>(curAssetPath);
                GraphicsSettings.renderPipelineAsset = pipelineAsset;
                PlayerSettings.colorSpace = ColorSpace.Linear;
            }
        }
    }
}
