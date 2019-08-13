﻿using UnityEngine;                                      //Debug
using UnityEditor;                                      //AssetPostProcessor
using UnityEngine.Rendering;                            //GraphicsSettings
using UnityEngine.Experimental.Rendering.HDPipeline;    //HDRenderPipelineAsset

public class HDRPPostProcessor : AssetPostprocessor
{
    static void OnPostprocessAllAssets(string[] importedAssets, string[] deletedAssets,
        string[] movedAssets, string[] movedFromAssetPaths)
    {
        Debug.Log("HDRPPostProcessor::OnPostprocessAllAssets");
        if (null != GraphicsSettings.renderPipelineAsset) {
            return;
        }
        
        int numImportedAssets = importedAssets.Length;
        for (int i=0;i< numImportedAssets; ++i) {
            string curAssetPath = importedAssets[i];

            Debug.Log("     Running---- " + curAssetPath);
            if (curAssetPath == "Assets/RenderPipeline/HDRP/HDRenderPipelineAsset.asset") {
                HDRenderPipelineAsset pipelineAsset = AssetDatabase.LoadAssetAtPath<HDRenderPipelineAsset>(curAssetPath);
                GraphicsSettings.renderPipelineAsset = pipelineAsset;

                PlayerSettings.colorSpace = ColorSpace.Linear;
                Debug.Log("     ColorSpace: " + PlayerSettings.colorSpace.ToString());

            }
        }
        AssetDatabase.SaveAssets();
        AssetDatabase.Refresh();

    }
}
