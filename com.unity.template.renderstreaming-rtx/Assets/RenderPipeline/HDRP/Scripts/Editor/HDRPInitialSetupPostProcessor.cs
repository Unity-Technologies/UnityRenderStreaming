﻿using UnityEngine;                                      //Debug
using UnityEditor;                                      //AssetPostProcessor
using UnityEngine.Rendering;                            //GraphicsSettings
#if UNITY_2019_3_OR_NEWER
using UnityEngine.Rendering.HighDefinition;             //HDRenderPipelineAsset
#else
using UnityEngine.Experimental.Rendering.HDPipeline;    //HDRenderPipelineAsset
#endif

public class HDRPPostProcessor : AssetPostprocessor
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

            if (curAssetPath == "Assets/RenderPipeline/HDRP/HDRenderPipelineAsset.asset") {
                HDRenderPipelineAsset pipelineAsset = AssetDatabase.LoadAssetAtPath<HDRenderPipelineAsset>(curAssetPath);
                GraphicsSettings.renderPipelineAsset = pipelineAsset;

                PlayerSettings.colorSpace = ColorSpace.Linear;
            }
        }
    }
}
